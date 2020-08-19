#include "iocp.h"

/*CSocketContext*/
bool CSocketContext::HandleRecvData(CSocketBuffer* pBuffer, DWORD dwTrans, CIocpTcpServer* pServer)
{
	if (dwTrans <= 0)
	{
		myLogConsoleI("%s �׽���%d�Ͽ�����", __FUNCTION__, pBuffer->m_hSocket);
		return false;
	}

	CAutoLock lock(&m_lock);
	m_llPendingRecvs--;
	if (pBuffer->m_llSerialNo != m_llCurrSerialNo)
	{
		pBuffer->m_pNext = nullptr;
		CSocketBuffer* pTemp = nullptr;
		CSocketBuffer* pHead = m_pWaitingRecv;
		while (pHead)
		{
			if (pBuffer->m_llSerialNo > pHead->m_llSerialNo)
			{
				break;
			}
			pTemp = pHead;
			pHead = pHead->m_pNext;
		}
		if (!pTemp)
		{
			/*pBufferӦ�ò��ڱ�ͷ*/
			pBuffer->m_pNext = m_pWaitingRecv;
			m_pWaitingRecv = pBuffer;
		}
		else
		{
			/*pBufferӦ�ò��ڱ���*/
			pBuffer->m_pNext = pTemp->m_pNext;
			pTemp->m_pNext = pBuffer;
		}
	}
	else
	{
		pBuffer->m_pNext = m_pWaitingRecv;
		m_pWaitingRecv = pBuffer;
	}

	CSocketBuffer* pDataBuffer = m_pWaitingRecv;
	while (pDataBuffer && pDataBuffer->m_pBuffer && (pDataBuffer->m_llSerialNo == m_llCurrSerialNo))
	{
		if (m_nRecvDataLen + dwTrans >= 8192)
		{
			break;
		}
		m_pWaitingRecv = pDataBuffer->m_pNext;
		m_llCurrSerialNo++;
		memcpy(m_recvDataBuff + m_nRecvDataLen, pDataBuffer->m_pBuffer->GetBuffer(), dwTrans);
		m_nRecvDataLen += dwTrans;

		if (pServer->m_nFlag & CPS_FLAG_MSG_HEAD)
		{
			if (m_nRecvDataLen > sizeof(NetPacketHead))
			{
				NetPacketHead* pHeader = (NetPacketHead*)(&m_recvDataBuff);
				if (!CheckHeader(pHeader))
				{
					return false;
				}
				if (m_nRecvDataLen >= (pHeader->nDataLen + sizeof(NetPacketHead)))
				{
					m_nRecvDataLen -= (pHeader->nDataLen + sizeof(NetPacketHead));
					IDataBuffer* pHandleData = m_pAllocator->AllocateDataBuffer(pHeader->nDataLen);
					memcpy(pHandleData->GetBuffer(), (BYTE*)(&m_recvDataBuff) + sizeof(NetPacketHead), pHeader->nDataLen);
					memmove((BYTE*)&m_recvDataBuff, (BYTE*)(&m_recvDataBuff) + pHeader->nDataLen + sizeof(NetPacketHead), (8192 - pHeader->nDataLen - sizeof(NetPacketHead)));
					pServer->OnDataHandle(this, pHandleData);
				}
			}
		}
		else
		{
			m_nRecvDataLen -= dwTrans;
			IDataBuffer* pHandleData = m_pAllocator->AllocateDataBuffer(dwTrans);
			memcpy(pHandleData->GetBuffer(), &m_recvDataBuff, dwTrans);
			memmove((BYTE*)&m_recvDataBuff, (BYTE*)(&m_recvDataBuff) + dwTrans, (8192 - dwTrans));
			pServer->OnDataHandle(this, pHandleData);
		}
		pDataBuffer = m_pWaitingRecv;
	}
	return true;
}

bool CSocketContext::CheckHeader(NetPacketHead* pHeader)
{
	if (!pHeader)
	{
		return false;
	}
	if (pHeader->nDataLen >= 8192)
	{
		return false;
	}
	return true;
}

/*CIocpTcpServer*/
CIocpTcpServer::CIocpTcpServer(DWORD dwFlag):
	m_nFlag(dwFlag)
{
	m_nPort = 0;
	m_bShutdown = false;

	m_hCompletionPort = INVALID_HANDLE_VALUE;
	::memset(m_hWaitHandle, 0, 2 * sizeof(HANDLE));
	::memset(m_szIp, 0, 16);
}

CIocpTcpServer::~CIocpTcpServer()
{

}

bool CIocpTcpServer::InitializeMembers()
{
	m_pAllocator = new CDataBufferMgr;
	if (!m_pAllocator)
	{
		return false;
	}

	m_pSocketBufferMgr = new CSocketBufferMgr;
	if (!m_pSocketBufferMgr || !m_pSocketBufferMgr->Init(m_pAllocator))
	{
		return false;
	}

	m_pSocketContextMgr = new CSocketContextMgr;
	if (!m_pSocketContextMgr)
	{
		return false;
	}

	return true;
}

bool CIocpTcpServer::Shutdown()
{
	m_bShutdown = true;

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	for (DWORD i = 0; i < siSysInfo.dwNumberOfProcessors * 2; i++)
	{
		::PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, NULL);
	}
	for (DWORD i = 0; i < siSysInfo.dwNumberOfProcessors * 2; i++)
	{
		if (m_socketThread[i].joinable())
		{
			m_socketThread[i].join();
		}
	}
	if (m_acceptThread.joinable())
	{
		m_acceptThread.join();
	}

	EndWorkerPool();

	m_pSocketBufferMgr->FreeSocketBuffer();
	SAFE_DELETE(m_pSocketBufferMgr);

	m_pSocketContextMgr->FreeSocketContext();
	SAFE_DELETE(m_pSocketContextMgr);

	SAFE_DELETE(m_pAllocator);

	SAFE_RELEASE_HANDLE(m_hCompletionPort);
	SAFE_RELEASE_HANDLE(m_hWaitHandle[0]);
	SAFE_RELEASE_HANDLE(m_hWaitHandle[1]);

	::memset(m_szIp, 0, 16);
	m_nPort = 0;
	myLogConsoleI("%s ����ر�", __FUNCTION__);

	::WSACleanup();
	return true;
}

bool CIocpTcpServer::EndWorkerPool()
{
	m_cond.notify_all();
	for (auto& iter : m_mapWorkerThreadCtx)
	{
		std::thread& thread = iter.second->m_thread;
		if (thread.joinable())
		{
			thread.join();
		}
	}
	for (auto& iter : m_mapWorkerThreadCtx)
	{
		OnWorkerExit(iter.second->m_pContext);
		delete iter.second;
	}
	m_mapWorkerThreadCtx.clear();
	return true;
}

bool CIocpTcpServer::BeginBindListen(const char* lpSzIp, UINT nPort, UINT nInitAccepts, UINT nMaxAccpets)
{
	m_nPort = nPort;
	lstrcpy(m_szIp, (TCHAR*)lpSzIp);
	m_nInitAccepts = nInitAccepts;

	bool bRet = m_pSocketContextMgr->Init(m_pAllocator, m_pSocketBufferMgr);
	if (!bRet)
	{
		return false;
	}

	SOCKADDR_IN sock_in;
	::memset(&sock_in, 0, sizeof(SOCKADDR_IN));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = ::ntohs(m_nPort);
	sock_in.sin_addr.S_un.S_addr = inet_addr((char*)m_szIp);

	int nRet = ::bind(m_pSocketContextMgr->GetListenSocket(), (SOCKADDR*)& sock_in, sizeof(sock_in));
	if (SOCKET_ERROR == nRet)
	{
		myLogConsoleE("%s �׽���bind����", __FUNCTION__);
		::closesocket(m_pSocketContextMgr->GetListenSocket());
		::WSACleanup();
		return false;
	}

	nRet = ::listen(m_pSocketContextMgr->GetListenSocket(), SOMAXCONN);
	if (SOCKET_ERROR == nRet)
	{
		myLogConsoleE("%s �׽���listen����", __FUNCTION__);
		::closesocket(m_pSocketContextMgr->GetListenSocket());
		::WSACleanup();
		return false;
	}

	::CreateIoCompletionPort((HANDLE)m_pSocketContextMgr->GetListenSocket(), m_hCompletionPort, (DWORD)NULL, 0);
	::WSAEventSelect(m_pSocketContextMgr->GetListenSocket(), m_pSocketContextMgr->GetAcceptHandle(), FD_ACCEPT);

	m_acceptThread = std::thread([&] {this->AcceptThreadFunc(); });

	return true;
}

bool CIocpTcpServer::BeginThreadPool(UINT nThreads)
{
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	for (DWORD i = 0; i < siSysInfo.dwNumberOfProcessors * 2; i++)
	{
		m_socketThread[i] = std::thread([&] {this->SocketThreadFunc(); });
	}

	for (UINT i = 0; i < nThreads; i++)
	{
		CThreadContext* pThreadCtx = new CThreadContext;
		pThreadCtx->m_pContext = OnWorkerStart();
		pThreadCtx->m_thread = std::thread([&] {this->WorkerThreadFunc(); });
		std::thread::id tid = pThreadCtx->m_thread.get_id();
		_Thrd_id_t* pid = reinterpret_cast<_Thrd_id_t*>(&tid);
		m_mapWorkerThreadCtx.insert(std::make_pair((DWORD)*pid, pThreadCtx));
	}

	return true;
}

bool CIocpTcpServer::InitializeIo()
{
	WSADATA wsaData;
	auto wsaRet = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != wsaRet)
	{
		return false;
	}

	m_hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (INVALID_HANDLE_VALUE == m_hCompletionPort)
	{
		myLogConsoleE("%s m_hCompletionPort�����Ч", __FUNCTION__);
		return false;
	}

	return true;
}

bool CIocpTcpServer::Initialize(const char* lpSzIp, UINT nPort, UINT nInitAccepts, UINT nMaxAccpets, UINT nThreads, UINT nMaxConnections)
{
	if (!InitializeMembers())
	{
		myLogConsoleE("%s InitializeMembersʧ��", __FUNCTION__);
		return false;
	}
	if (!InitializeIo())
	{
		myLogConsoleE("%s InitializeIoʧ��", __FUNCTION__);
		return false;
	}
	if (!BeginBindListen(lpSzIp, nPort, nInitAccepts, nMaxAccpets))
	{
		myLogConsoleE("%s BeginListenʧ��", __FUNCTION__);
		return false;
	}
	if (!BeginThreadPool(nThreads))
	{
		myLogConsoleE("%s BeginThreadPoolʧ��", __FUNCTION__);
		return false;
	}
	return true;
}

bool CIocpTcpServer::PostAccept(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	CSocketListenContext* pListen = m_pSocketContextMgr->GetListenCtx();
	if (!pListen)
	{
		return false;
	}

	pBuffer->m_ioType = IoType::enIoAccept;
	pBuffer->m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == pBuffer->m_hSocket)
	{
		return false;
	}

	// ����Ϊ������
	unsigned long ul = 1;
	int nRet = ioctlsocket(pBuffer->m_hSocket, FIONBIO, (unsigned long*)& ul);
	if (nRet != 0)
	{
		return false;
	}

	DWORD dwBytes = 0;
	BYTE acceptEx[64];
	ZeroMemory(&acceptEx, 64);
	BOOL bRet = pListen->m_lpfnAcceptEx(
		pListen->m_hSocket,
		pBuffer->m_hSocket,
		&acceptEx,
		/*����Accept��ʱ��������������ӵĵ�һ�����ݣ�ͬʱ����ط��ᵼ�»�ȡ��ɶ˿ڶ�������ʱ��Ϊ0*/
		/*pBuffer->m_nBufferLen - ((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		&(pBuffer->m_ol)
	);
	dwWSAError = ::WSAGetLastError();
	if (FALSE == bRet && WSA_IO_PENDING != dwWSAError)
	{
		return false;
	}
	return true;
}

bool CIocpTcpServer::PostRecv(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	CAutoLock lock(&(pContext->m_lock));
	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	pBuffer->m_ioType = IoType::enIoRead;
	pBuffer->m_hSocket = pContext->m_hSocket;
	pBuffer->m_llSerialNo = pContext->m_llNextSerialNo;

	WSABUF wsaBuf = { 0 };
	wsaBuf.buf = (CHAR*)pBuffer->m_pBuffer->GetBuffer();
	wsaBuf.len = pBuffer->m_pBuffer->GetBufferLen();

	DWORD dwRet = ::WSARecv(pContext->m_hSocket, &wsaBuf, 1, &dwBytes, &dwFlags, &pBuffer->m_ol, NULL);
	dwWSAError = ::WSAGetLastError();
	if (NO_ERROR != dwRet && WSA_IO_PENDING != dwWSAError)
	{
		myLogConsoleW("%s WSARecv ʧ��", __FUNCTION__);
		return false;
	}
	pContext->m_llNextSerialNo++;
	pContext->m_llPendingRecvs++;
	return true;
}

bool CIocpTcpServer::PostSend(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	if (TRUE == pContext->m_bClosing)
	{
		myLogConsoleW("%s denied because of the socket:%d is closing", __FUNCTION__, pContext->m_hSocket);
		return false;
	}

	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	pBuffer->m_ioType = IoType::enIoWrite;
	pBuffer->m_hSocket = pContext->m_hSocket;

	WSABUF wsaBuf = { 0 };
	wsaBuf.buf = (CHAR*)pBuffer->m_pBuffer->GetBuffer();
	wsaBuf.len = pBuffer->m_pBuffer->GetBufferLen();

	DWORD dwRet = ::WSASend(pContext->m_hSocket, &wsaBuf, 1, &dwBytes, dwFlags, &pBuffer->m_ol, NULL);
	dwWSAError = ::WSAGetLastError();
	if (NO_ERROR != dwRet && WSA_IO_PENDING != dwWSAError)
	{
		myLogConsoleW("%s WSASend ʧ�� WSAGetLastError:%ld", __FUNCTION__, dwWSAError);
		return false;
	}
	pContext->m_llPendingSends++;
	return true;
}

bool CIocpTcpServer::ConnectOneServer(const std::string strIp, const int nPort)
{
	SOCKET hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hSocket == INVALID_SOCKET)
	{
		return false;
	}

	CSocketContext* pContext = m_pSocketContextMgr->AllocateSocketContext(hSocket);
	if (!pContext)
	{
		return false;
	}
	pContext->m_hSocket = hSocket;
	::CreateIoCompletionPort((HANDLE)pContext->m_hSocket, m_hCompletionPort, (DWORD)pContext, 0);

	DWORD dwBytes = 0;
	DWORD dwSendBytes = 0;
	GUID guid = WSAID_CONNECTEX;

	LPFN_CONNECTEX lpfnConnectEx = nullptr;
	if (SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL))
	{
		return false;
	}

	SOCKADDR_IN svrAddr;
	memset(&svrAddr, 0, sizeof(SOCKADDR_IN));
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(0);
	svrAddr.sin_addr.s_addr = INADDR_ANY;

	int ret = ::bind(pContext->m_hSocket, (SOCKADDR*)&svrAddr, sizeof(svrAddr));
	if (SOCKET_ERROR == ret)
	{
		return false;
	}

	svrAddr.sin_port = htons(nPort);
	svrAddr.sin_addr.S_un.S_addr = inet_addr(strIp.c_str());

	WSAOVERLAPPED wsaOverlapped;
	ZeroMemory(&wsaOverlapped, sizeof(wsaOverlapped));
	BOOL bRet = lpfnConnectEx(
		pContext->m_hSocket,
		(const SOCKADDR*)&svrAddr,
		sizeof(SOCKADDR_IN),
		NULL,
		NULL,
		&dwSendBytes,
		&wsaOverlapped);
	DWORD dwError = ::WSAGetLastError();
	if (!bRet && dwError != WSA_IO_PENDING)
	{
		return false;
	}

	return true;
}

bool CIocpTcpServer::DisconnectServer(SOCKET hSocket)
{
	DWORD dwBytes = 0;
	DWORD dwSendBytes = 0;
	GUID guid = WSAID_DISCONNECTEX;

	LPFN_DISCONNECTEX lpfnDisconnectEx = nullptr;
	if (SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &lpfnDisconnectEx, sizeof(lpfnDisconnectEx), &dwBytes, NULL, NULL))
	{
		return false;
	}

	WSAOVERLAPPED wsaOverlapped;
	ZeroMemory(&wsaOverlapped, sizeof(wsaOverlapped));
	DWORD dwFlags = TF_REUSE_SOCKET;
	BOOL bRet = lpfnDisconnectEx(
		hSocket,
		&wsaOverlapped,
		dwFlags,
		0);
	DWORD dwError = ::WSAGetLastError();
	if (!bRet && dwError != WSA_IO_PENDING)
	{
		return false;
	}

	CSocketContext* pContext = m_pSocketContextMgr->GetCtx(hSocket);
	m_pSocketContextMgr->ReleaseSocketContext(pContext);
	return true;
}

bool CIocpTcpServer::CloseClient(CSocketContext* pContext)
{
	CSocketBuffer* pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1);
	if (pBuffer)
	{
		pBuffer->m_ioType = IoType::enIoClose;
		BOOL bResult = ::PostQueuedCompletionStatus(m_hCompletionPort, 1, (ULONG_PTR)pContext, &(pBuffer->m_ol));
		return (TRUE == bResult) ? true : false;
	}
	return false;
}

bool CIocpTcpServer::SendData(SOCKET hSocket, const void* pDataPtr, int nDataLen, UINT uiMsgType)
{
	CSocketContext* pContext = m_pSocketContextMgr->GetCtx(hSocket);
	if (pContext)
	{
		return SendData(pContext, pDataPtr, nDataLen);
	}
	return false;
}

bool CIocpTcpServer::SendData(CSocketContext* pContext, const void* pDataPtr, int nDataLen)
{
	CSocketBuffer* pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(nDataLen);
	if (pBuffer && pBuffer->m_pBuffer && pBuffer->m_pBuffer->GetDataLen() > 0)
	{
		memcpy(pBuffer->m_pBuffer->GetBuffer(), pDataPtr, nDataLen);
		DWORD dwError = 0;
		return PostSend(pContext, pBuffer, dwError);
	}
	return false;
}

bool CIocpTcpServer::SendPbData(CSocketContext* pContext, const google::protobuf::Message& pdata)
{
	if (pdata.ByteSizeLong() >= 8192)
	{
		myLogConsoleW("%s ������pb���ݹ���", __FUNCTION__);
		return false;
	}
	char szBuff[8192] = { 0 };
	pdata.SerializeToArray(szBuff, pdata.GetCachedSize());
	return SendData(pContext, szBuff, pdata.GetCachedSize());
}

void CIocpTcpServer::HandleIo(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (!pContext)
	{
		if (NO_ERROR != dwError)
		{
			if (IoType::enIoAccept != pBuffer->m_ioType)
			{
				/*�׽��ַ������󣬶Ͽ����ӣ��ͷ������Դ*/
				myLogConsoleW("%s �׽���%d��������", __FUNCTION__, pBuffer->m_hSocket);
			}
			else
			{
				/*�������׽����Ϸ������󣬷���������رռ����ĸ�socket�׽���*/
				SAFE_RELEASE_SOCKET(pBuffer->m_hSocket);
			}
			m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
		}
		else
		{
			/*���׽������ӽ���ʱ��dwKey = 0 dwTrans = 0 dwError = 0����ʱpContext��ΪNULL�����´��������������*/
			CSocketListenContext* pListen = m_pSocketContextMgr->GetListenCtx();
			if (IoType::enIoAccept == pBuffer->m_ioType && pListen)
			{
				m_pSocketBufferMgr->RemovePendingAccepts(pBuffer);
				HandleIoAccept(dwKey, pBuffer, dwTrans, dwError);
			}
			else
			{
				m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
			}
		}
		return;
	}

	/*�����׽����ϵĴ���*/
	if (NO_ERROR != dwError)
	{
		if (IoType::enIoAccept != pBuffer->m_ioType)
		{
			/*�׽��ַ������󣬶Ͽ����ӣ��ͷ������Դ*/
			myLogConsoleW("%s �׽���%d��������", __FUNCTION__, pBuffer->m_hSocket);
		}
		else
		{
			/*�������׽����Ϸ������󣬷���������رռ����ĸ�socket�׽���*/
			SAFE_RELEASE_SOCKET(pBuffer->m_hSocket);
		}
		m_pSocketContextMgr->ReleaseSocketContext(pContext);
		m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
		return;
	}

	/*�ֱ���I/O����*/
	switch (pBuffer->m_ioType)
	{
	case IoType::enIoAccept:
		HandleIoAccept(dwKey, pBuffer, dwTrans, dwError);
		break;
	case IoType::enIoRead:
		HandleIoRead(dwKey, pBuffer, dwTrans, dwError);
		break;
	case IoType::enIoWrite:
		HandleIoWrite(dwKey, pBuffer, dwTrans, dwError);
		break;
	case IoType::enIoClose:
		HandleIoClose(dwKey, pBuffer, dwTrans, dwError);
		break;
	default:
		HandleIoDefault(dwKey, pBuffer, dwTrans, dwError);
		break;
	}
}

void CIocpTcpServer::HandleIoDefault(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	// ��ʱ��������
}

void CIocpTcpServer::HandleIoAccept(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*����Accept����*/
	CSocketListenContext* pListen = m_pSocketContextMgr->GetListenCtx();
	if (!pListen)
	{
		return;
	}
	CSocketContext* pContext = m_pSocketContextMgr->AllocateSocketContext(pBuffer->m_hSocket);
	if (pContext)
	{
		/*���������������*/
		BOOL bRet = true;
		if (bRet)
		{
			BYTE getAcceptExSockaddr[64];
			ZeroMemory(&getAcceptExSockaddr, 64);
			LPSOCKADDR lpLocalAddr = NULL;
			LPSOCKADDR lpRemoteAddr = NULL;
			int nLocalLen = sizeof(SOCKADDR_IN);
			int nRemoteLen = sizeof(SOCKADDR_IN);
			pListen->m_lpfnGetAcceptExSockaddrs(
				&getAcceptExSockaddr,
				/*����Accept��ʱ��������������ӵĵ�һ�����ݣ�ͬʱ����ط��ᵼ�»�ȡ��ɶ˿ڶ�������ʱ��Ϊ0*/
				/*pBuffer->m_nBufferLen - ((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
				sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16,
				(LPSOCKADDR*)& lpLocalAddr,
				&nLocalLen,
				(LPSOCKADDR*)& lpRemoteAddr,
				&nRemoteLen
			);
			::memcpy(&pContext->m_local, lpLocalAddr, nLocalLen);
			::memcpy(&pContext->m_remote, lpRemoteAddr, nRemoteLen);

			// https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex
			// When the AcceptEx function returns, the socket sAcceptSocket is in the default state for a connected socket.
			// The socket sAcceptSocket does not inherit the properties of the socket associated with sListenSocket parameter until SO_UPDATE_ACCEPT_CONTEXT is set on the socket.
			int nRet = 0;
			nRet = setsockopt(pContext->m_hSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)& pListen->m_hSocket, sizeof(pListen->m_hSocket));
			if (nRet != 0)
			{
				m_pSocketContextMgr->ReleaseSocketContext(pContext);
				return;
			}

			/*���½����ӵ��׽���socket����ɶ˿ڰ�*/
			::CreateIoCompletionPort((HANDLE)pContext->m_hSocket, m_hCompletionPort, (DWORD)pContext, 0);

			/*Ϊ������Ͷ��һ��Read����*/
			CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
			if (pNewBuffer)
			{
				DWORD dwError = 0;
				if (!PostRecv(pContext, pNewBuffer, dwError))
				{
					/*Ͷ��ʧ�ܣ��ر�������*/
					m_pSocketContextMgr->ReleaseSocketContext(pContext);
					m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
					myLogConsoleW("%s �������׽���%dͶ��Read����ʧ�ܣ��Ͽ�����", __FUNCTION__, pContext->m_hSocket);
				}
			}
		}
		else
		{
			/*�ﵽ�涨����������ƣ��ر����ӣ��ͷ����������Դ*/
			myLogConsoleW("%s �ﵽ�涨�������������%d���ر�����", __FUNCTION__, 1000);
			m_pSocketContextMgr->ReleaseSocketContext(pContext);
		}
	}
	else
	{
		/*��Դ����ʧ�ܣ��رո�����*/
		myLogConsoleW("%s ��Դ����ʧ�ܣ��رո�����", __FUNCTION__);
		SAFE_RELEASE_SOCKET(pBuffer->m_hSocket);
	}

	/*Accept I/O������ɣ��ͷ�pBuffer*/
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);

	/*֪ͨAccept�߳��е�m_hRepostHandle�¼�����Ͷ��һ��Accept����*/
	pListen->NotifyRepostAccepts();
}

void CIocpTcpServer::HandleIoRead(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*����Read����*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		bool bRet = pContext->HandleRecvData(pBuffer, dwTrans, this);
		if (!bRet)
		{
			/*���Ӵ����������ʧ�ܣ��ر�����*/
			m_pSocketContextMgr->ReleaseSocketContext(pContext);
			//myLogConsoleI("%s I/O�����������쳣���Է��׽���%d�Ͽ�", __FUNCTION__, pContext->m_hSocket);
		}
		else
		{
			CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
			DWORD dwError = 0;
			if (!pNewBuffer || !PostRecv(pContext, pNewBuffer, dwError))
			{
				/*Ͷ�ݶ��¼�ʧ�ܣ��ر�����*/
				myLogConsoleI("%s I/O�����������쳣���Է��׽��ֶϿ�", __FUNCTION__);
				m_pSocketContextMgr->ReleaseSocketContext(pContext);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
			}
		}
	}
	else
	{
		myLogConsoleW("%s pContextΪ��!!!", __FUNCTION__);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::HandleIoWrite(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*����Write����*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		if (0 == dwTrans)
		{
			/*I/Oд���������쳣���رո��׽��ֵ����ӣ��ͷ������Դ*/
			myLogConsoleI("%s I/Oд���������쳣���Է��׽���%d�Ͽ�", __FUNCTION__, pContext->m_hSocket);
			m_pSocketContextMgr->ReleaseSocketContext(pContext);
		}
		else
		{
			CAutoLock lock(&pContext->m_lock);
			pContext->m_llPendingSends--;
		}
	}
	else
	{
		myLogConsoleW("%s pContextΪ��!!!", __FUNCTION__);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::HandleIoClose(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*����Close����*/
	BOOL bResult = FALSE;
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		CAutoLock lock(&pContext->m_lock);
		if (!pContext->m_bClosing)
		{
			CAutoLock lock(&pContext->m_lock);
			pContext->m_bClosing = TRUE;
		}
		if (0 == pContext->m_llPendingSends)
		{
			bResult = TRUE;
			int nRet = NO_ERROR;
			nRet = ::shutdown(pContext->m_hSocket, SD_SEND);
			if (nRet != NO_ERROR)
			{
				int nError = WSAGetLastError();
				myLogConsoleE("%s shutdown failed with errorcode %d", __FUNCTION__, nError);
			}
		}
		else
		{
			CloseClient(pContext);
		}
	}
	else
	{
		myLogConsoleW("%s pContextΪ��!!!", __FUNCTION__);
	}
	if (bResult)
	{
		m_pSocketContextMgr->RemovePendingCloses(pContext->m_hSocket);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::OnDataHandle(CSocketContext* pContext, IDataBuffer* pBuffer)
{
	std::lock_guard<std::mutex> lk(m_lock);
	m_recvPacket.emplace_back(pContext, pBuffer);
	m_cond.notify_one();
}

void CIocpTcpServer::DispatchData(NetPacket* pNP)
{
	//myLogConsoleI("recv data:[%s]", pNP->GetData());
	OnRequest((void*)pNP, GetWorkerContext());
	pNP->Release();
}

void CIocpTcpServer::OnRequest(void* p1, void* p2)
{
	NetPacket* pNP = (NetPacket*)p1;
	myLogConsoleI("recv:[%s]", (char*)pNP->GetData());
	//std::string str = "Slow,Normal,Fast,SlowScore,FastScore,Multi,Calculate,TimeLineCycle,RandomFish,DropItem,RookieFix,DraAttack,BoardC,BoardW,Together,RemoveA,RemoveB,BagNums,GiveVlimit,MailVal,MailNums,MailInterval,WorldInterval,WnLimit,RoomInterval,UpdateSilver,SpeventTime,RemoveC,RegSilver,BonusPer,SpeventTime2,QuestMaxnum,QuestChange,NormalFix,BreakFix,ChargeFix,FirstSkill,Refresh,RefreshNum,MuseumPerlimit,TreasureVip,MistLimit,LjPieces,Basiladd,TideTime,BRNN20,NewFix,NewReliefSilver,NewReliefTimes,NewReliefLimit,GuideLottery,SvrStopNotice,TimeLineCycle1,RandomFish1,QuickOpen,ScoreBoardLimit,CHZZ100,BonusPerMax,BonusPerMin,GoldPerMax,GoldPerMin,BonusPerMaxModify,BonusPerMinModify,GoldPerMaxModify,GoldPerMinModify,NewerStockNum,NewerStockExp,3000,350,1000,1600,200,2000,3000,693000,688000,15000,1.06|100|30,200000,15|15,1|9,8|13,40,110,15,2,30,100,600000,10000,5,1500,10000,7000,15,150,40,2000,3,3,1.015|100|300,1.03|100|5,0.01|100|5,1.0|1.0|1.0|1.0|1.0|1.0|1.0|1.0|1.0,0|6|12|18,30,8,2,1000,200,10,10,0,1.045|1.03|1.015,5000,3,0,10000,����������ά���У�Ԥ��ά��ʱ��Ϊ2Сʱ������ά����Ϻ����µ�¼ ��,86400000,86395000,5,5000000,0,60,50,11,9,30,15,6,4,100000,18750,";
	std::string str = "helloworld!!!!!!!";
	/*int nSize = pNP->GetBuffer()->GetDataLen();
	CBuffer buffer;
	buffer.Write(PBYTE(&nSize), sizeof(int));
	buffer.Write(pNP->GetBuffer()->GetData(), nSize);
	SendData(pNP->GetCtx(), buffer.GetBuffer(), buffer.GetBufferLen());*/
	SendData(pNP->GetCtx(), str.c_str(), str.size());
	CloseClient(pNP->GetCtx());
}

void* CIocpTcpServer::GetWorkerContext()
{
	DWORD dwThreadId = ::GetCurrentThreadId();
	return (void*)(m_mapWorkerThreadCtx[dwThreadId]->m_pContext);
}

void* CIocpTcpServer::OnWorkerStart()
{
	return nullptr;
}

void CIocpTcpServer::OnWorkerExit(void* pContext)
{

}

void CIocpTcpServer::AcceptThreadFunc()
{
	if (!m_pSocketBufferMgr)
	{
		return;
	}
	CSocketListenContext* pListenCtx = m_pSocketContextMgr->GetListenCtx();
	if (!pListenCtx)
	{
		myLogConsoleW("%s ListenContext is nullptr, Accept�߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
		return;
	}

	CSocketBuffer* pBuffer = nullptr;
	DWORD dwError = 0;

	for (UINT i = 0; i < m_nInitAccepts; i++)
	{
		pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
		if (!pBuffer)
		{
			myLogConsoleW("%s �߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}
		m_pSocketBufferMgr->InsertPendingAccepts(pBuffer);
		PostAccept(NULL, pBuffer, dwError);
	}

	int nEventCount = 0;
	m_hWaitHandle[nEventCount++] = m_pSocketContextMgr->GetAcceptHandle();
	m_hWaitHandle[nEventCount++] = m_pSocketContextMgr->GetRepostHandle();

	while (true)
	{
		if (m_bShutdown)
		{
			myLogConsoleW("%s Accept�߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}

		// ��ʱ��m_hWaitHandle���м�������Ԥ�����Ѿ�Ͷ��AcceptEx�����������֮ǰ��ʱ��ӣ�����ʧ��ֱ�ӷ���
		DWORD dwWaitRet = ::WSAWaitForMultipleEvents(nEventCount, m_hWaitHandle, FALSE, 5000, FALSE);
		if (WSA_WAIT_FAILED == dwWaitRet)
		{
			myLogConsoleW("%s Accept�߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
			Shutdown();
			return;
		}
		else if (WSA_WAIT_TIMEOUT == dwWaitRet)
		{
			// ��鵱ǰ���������AcceptEx I/O����ʱ��
			m_pSocketBufferMgr->CheckPendingAccpets();

			// ��鵱ǰ��������д��ر�����
			m_pSocketContextMgr->CheckPendingCloses();
		}
		else if(WSA_WAIT_EVENT_0 == dwWaitRet)
		{
			// ��ѯAccept�¼�����dwWaitRet����ֵ����λ��[WSA_WAIT_EVENT_0�� (WSA_WAIT_EVENT_0+ nEventCount - 1)]����Ӧ��������������������
			dwWaitRet = dwWaitRet - WSA_WAIT_EVENT_0;
			int nAddAcceptCounts = 0;
			if (0 == dwWaitRet)
			{
				WSANETWORKEVENTS wsaNetEvent;
				::WSAEnumNetworkEvents(pListenCtx->m_hSocket, m_hWaitHandle[dwWaitRet], &wsaNetEvent);
				if (FD_ACCEPT & wsaNetEvent.lNetworkEvents)
				{
					// ����FD_ACCEPT�����¼�����ζ��Ͷ�ݵ�Accept��������Ҫ����
					nAddAcceptCounts = 64;
				}
			}
			else if (1 == dwWaitRet)
			{
				nAddAcceptCounts = ::InterlockedExchange(&pListenCtx->m_nRepostCount, 0);;
			}
			else if (dwWaitRet >= (DWORD)nEventCount)
			{
				// �����¼������������������󣬹رշ���
				Shutdown();
				myLogConsoleI("%s Accept�߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
				return;
			}

			// ����AcceptͶ��
			int nCount = 0;
			while ((nCount++ < nAddAcceptCounts))
			{
				CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
				if (pNewBuffer)
				{
					dwError = 0;
					m_pSocketBufferMgr->InsertPendingAccepts(pNewBuffer);
					PostAccept(NULL, pNewBuffer, dwError);
				}
			}
		}
	}
	myLogConsoleI("%s Accept�߳�%d�˳�...", __FUNCTION__, GetCurrentThreadId());
	return;
}

void CIocpTcpServer::SocketThreadFunc()
{
	CSocketBuffer* pBuffer = NULL;
	DWORD dwKey = 0;
	DWORD dwTrans = 0;
	LPOVERLAPPED lpOverlapped = NULL;
	while (true)
	{
		if (m_bShutdown)
		{
			myLogConsoleW("%s�����߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}

		/*��ȡ��ɶ˿ڶ�������*/
		BOOL bRet = ::GetQueuedCompletionStatus(m_hCompletionPort, &dwTrans, (PULONG_PTR)& dwKey, &lpOverlapped, WSA_INFINITE);

		DWORD dwError = NO_ERROR;
		DWORD dwFlags = 0;
		if (FALSE == bRet)
		{
			if (!lpOverlapped)
			{
				continue;
			}

			CSocketContext* pContext = (CSocketContext*)dwKey;
			if (!pContext)
			{
				continue;
			}
			else
			{
				/*ȡ����ӦlpOverlapped��CSocketBuffer����I/O����*/
				pBuffer = CONTAINING_RECORD(lpOverlapped, CSocketBuffer, m_ol);
				if (!pBuffer)
				{
					continue;
				}
				SOCKET socket = INVALID_SOCKET;
				if (IoType::enIoAccept == pBuffer->m_ioType)
				{
					CSocketListenContext* pListen = m_pSocketContextMgr->GetListenCtx();
					if (pListen)
					{
						socket = pListen->m_hSocket;
					}
				}
				else
				{
					socket = pContext->m_hSocket;
				}
				/*��ѯ���׽ӿ���һ���ص�����ʧ�ܵ�ԭ��*/
				BOOL bResult = ::WSAGetOverlappedResult(socket, &(pBuffer->m_ol), &dwError, FALSE, &dwFlags);
				if (!bResult)
				{
					dwError = ::WSAGetLastError();
					myLogConsoleW("%s WSAGetOverlappedResult����ʧ��", __FUNCTION__);
					myLogConsoleW("%s GetQueuedCompletionStatus����False��������dwError��%d", __FUNCTION__, dwError);
				}
			}
		}
		else
		{
			if (!lpOverlapped)
			{
				myLogConsoleW("%s �����߳�%d�˳�...lpOverlapped is nullptr", __FUNCTION__, ::GetCurrentThreadId());
				return;
			}
			/*ȡ����ӦlpOverlapped��CSocketBuffer����I/O����*/
			pBuffer = CONTAINING_RECORD(lpOverlapped, CSocketBuffer, m_ol);
			if (!pBuffer)
			{
				continue;
			}
			if (0 == dwKey && IoType::enIoAccept != pBuffer->m_ioType)
			{
				myLogConsoleW("%s �����߳�%d�˳�...0 == dwTrans && 0 == dwKey && enIoAccept != pBuffer->m_ioType", __FUNCTION__, GetCurrentThreadId());
				return;
			}
		}

		/*����I/O����*/
		HandleIo(dwKey, pBuffer, dwTrans, dwError);
	}
	myLogConsoleI("%s �����߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
	return;
}

void CIocpTcpServer::WorkerThreadFunc()
{
	while (true)
	{
		std::deque<NetPacket> diptPacket;
		if (m_bShutdown)
		{
			myLogConsoleW("%s WorkerThreadFunc�߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}
		{
			std::unique_lock<std::mutex> uk(m_lock);
			m_cond.wait(uk, [this] {return !m_recvPacket.empty() || m_bShutdown; });
			std::swap(m_recvPacket, diptPacket);
		}

		for (auto& iter : diptPacket)
		{
			NetPacket& pNetPacket = iter;
			DispatchData(&pNetPacket);
		}
		diptPacket.clear();
	}
	myLogConsoleW("%s WorkerThreadFunc�߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
	return;
}
