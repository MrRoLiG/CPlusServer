#include "iocp.h"

/*CSocketContext*/
bool CSocketContext::HandleRecvData(CSocketBuffer* pBuffer, DWORD dwTrans, CIocpTcpServer* pServer)
{
	if (dwTrans <= 0)
	{
		myLogConsoleW("%s �׽���%d�Ͽ�����", __FUNCTION__, pBuffer->m_hSocket);
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
					IDataBuffer* pHandleData = m_pAllocator->AllocateDataBuffer(pHeader->nDataLen);
					memcpy(pHandleData->GetBuffer(), (BYTE*)(&m_recvDataBuff) + sizeof(NetPacketHead), pHeader->nDataLen);
					memmove((BYTE*)&m_recvDataBuff, (BYTE*)(&m_recvDataBuff) + pHeader->nDataLen + sizeof(NetPacketHead), (8192 - pHeader->nDataLen - sizeof(NetPacketHead)));
					m_nRecvDataLen -= (pHeader->nDataLen + sizeof(NetPacketHead));
					pServer->OnDataHandle(this, pHandleData);
				}
			}
		}
		else
		{
			IDataBuffer* pHandleData = m_pAllocator->AllocateDataBuffer(dwTrans);
			memcpy(pHandleData->GetBuffer(), &m_recvDataBuff, dwTrans);
			memmove((BYTE*)&m_recvDataBuff, (BYTE*)(&m_recvDataBuff) + dwTrans, (8192 - dwTrans));
			m_nRecvDataLen -= dwTrans;
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

bool CIocpTcpServer::InitializeMembers(UINT nMaxConnections)
{
	m_pAllocator = new CDataBufferMgr();
	if (!m_pAllocator)
	{
		return false;
	}

	m_pSocketBufferMgr = new CSocketBufferMgr();
	if (!m_pSocketBufferMgr || !m_pSocketBufferMgr->Init(m_pAllocator))
	{
		return false;
	}

	m_pSocketContextMgr = new CSocketContextMgr();
	if (!m_pSocketContextMgr)
	{
		return false;
	}
	if (!m_pSocketContextMgr->Init(m_pAllocator, m_pSocketBufferMgr, nMaxConnections))
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

	SOCKADDR_IN sock_in;
	::memset(&sock_in, 0, sizeof(SOCKADDR_IN));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = ::ntohs(m_nPort);
	inet_pton(AF_INET, (PCSTR)m_szIp, &sock_in.sin_addr);

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

	return true;
}

bool CIocpTcpServer::BeginThreadPool(UINT nThreads/* = 0*/)
{
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	m_acceptThread = std::thread([&] {this->AcceptThreadFunc(); });
	for (DWORD i = 0; i < siSysInfo.dwNumberOfProcessors * 2; i++)
	{
		m_socketThread[i] = std::thread([&] {this->SocketThreadFunc(); });
	}
	for (UINT i = 0; i < nThreads; i++)
	{
		CThreadContext* pThreadCtx = new CThreadContext();
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
	if (!InitializeIo())
	{
		myLogConsoleE("%s InitializeIoʧ��", __FUNCTION__);
		return false;
	}
	if (!InitializeMembers(nMaxConnections))
	{
		myLogConsoleE("%s InitializeMembersʧ��", __FUNCTION__);
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

	// acceptEx�����趨Ϊ1024��������ݰ���С���ټ��ϱ�����Զ��SOCKADDR_IN����
	DWORD dwBytes = 0;
	BYTE acceptEx[1024 + 2 * (sizeof(SOCKADDR_IN) + 16)];
	ZeroMemory(&acceptEx, 1024 + 2 * (sizeof(SOCKADDR_IN) + 16));
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
	if (pContext->m_bClosing || pContext->m_bDelayClose)
	{
		myLogConsoleW("%s �׽���%d���ڹر�", __FUNCTION__, pContext->m_hSocket);
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

bool CIocpTcpServer::PostConn(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	DWORD dwBytes = 0;
	DWORD dwSendBytes = 0;
	GUID guid = WSAID_CONNECTEX;
	LPFN_CONNECTEX lpfnConnectEx = nullptr;
	if (SOCKET_ERROR == ::WSAIoctl(pContext->m_hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL))
	{
		myLogConsoleE("%s ��ȡConnectEx����ָ��ʧ��", __FUNCTION__);
		return false;
	}
	pBuffer->m_hSocket = pContext->m_hSocket;
	pBuffer->m_ioType = IoType::enIoConn;

	BOOL bRet = lpfnConnectEx(
		pContext->m_hSocket,
		(const SOCKADDR*)&pContext->m_remote,
		sizeof(SOCKADDR_IN),
		NULL,
		NULL,
		&dwSendBytes,
		&pBuffer->m_ol);
	DWORD dwError = ::WSAGetLastError();
	if (!bRet && dwError != WSA_IO_PENDING)
	{
		return false;
	}
	return true;
}

bool CIocpTcpServer::PostClose(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	if (pContext->m_bClosing)
	{
		myLogConsoleW("%s �׽���%d���ڹر�", __FUNCTION__, pContext->m_hSocket);
		return false;
	}
	pBuffer->m_ioType = IoType::enIoClose;
	pBuffer->m_hSocket = pContext->m_hSocket;
	DWORD dwBytes = 0;
	DWORD dwSendBytes = 0;
	GUID guid = WSAID_DISCONNECTEX;
	LPFN_DISCONNECTEX lpfnDisconnectEx = nullptr;
	if (SOCKET_ERROR == ::WSAIoctl(pBuffer->m_hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &lpfnDisconnectEx, sizeof(lpfnDisconnectEx), &dwBytes, NULL, NULL))
	{
		myLogConsoleE("%s ��ȡDisconnectEx����ָ��ʧ��", __FUNCTION__);
		return false;
	}
	DWORD dwFlags = TF_REUSE_SOCKET;
	BOOL bRet = lpfnDisconnectEx(
		pBuffer->m_hSocket,
		&pBuffer->m_ol,
		dwFlags,
		0);
	DWORD dwError = ::WSAGetLastError();
	if (!bRet && dwError != WSA_IO_PENDING)
	{
		myLogConsoleE("%s DisconnectExʧ�� %d WSAGetLastError:%ld", __FUNCTION__, pContext->m_hSocket, dwError);
		return false;
	}
	pContext->m_bClosing = TRUE;
	m_pSocketContextMgr->InsertPendingCloses(pContext);
	return true;
}

bool CIocpTcpServer::CloseClient(SOCKET hSocket)
{
	if (INVALID_SOCKET == hSocket)
	{
		return false;
	}
	CSocketContext* pCloseContext = m_pSocketContextMgr->GetCtx(hSocket);
	if (pCloseContext)
	{
		bool bClose = false;
		{
			CAutoLock lk(&pCloseContext->m_lock);
			if (!pCloseContext->m_pWaitingSend && pCloseContext->m_llPendingSends <= 0)
			{
				bClose = true;
			}
		}
		if (bClose)
		{
#if 1
			CSocketBuffer * pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
			if (pBuffer)
			{
				DWORD dwError = NO_ERROR;
				bool bRet = PostClose(pCloseContext, pBuffer, dwError);
				if (!bRet)
				{
					m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
				}
				return bRet;
			}
#else
			int nRet = NO_ERROR;
			nRet = ::shutdown(pContext->m_hSocket, SD_SEND);
			if (nRet != NO_ERROR)
			{
				int nError = WSAGetLastError();
				myLogConsoleE("%s shutdown failed with errorcode %d", __FUNCTION__, nError);
			}
#endif
		}
		pCloseContext->m_bDelayClose = TRUE;
	}
	return false;
}

bool CIocpTcpServer::SendData(SOCKET hSocket, const void* pDataPtr, int nDataLen)
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
#if 0
	CSocketBuffer* pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(nDataLen);
	if (pBuffer && pBuffer->m_pBuffer && pBuffer->m_pBuffer->GetDataLen() > 0)
	{
		memcpy(pBuffer->m_pBuffer->GetBuffer(), pDataPtr, nDataLen);
		DWORD dwError = 0;
		return PostSend(pContext, pBuffer, dwError);
	}
	return FALSE;
#else
	bool bRet = false;
	CSocketBuffer* pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(nDataLen);
	if (pBuffer)
	{
		memcpy(pBuffer->m_pBuffer->GetBuffer(), pDataPtr, nDataLen);
		{
			CAutoLock lk(&pContext->m_lock);
			if (pContext->m_llPendingSends > 0)
			{
				if (!pContext->m_pWaitingSend)
				{
					pContext->m_pWaitingSend = pBuffer;
				}
				else
				{
					CSocketBuffer* pSendBuffer = pContext->m_pWaitingSend;
					while (pSendBuffer->m_pNext)
					{
						pSendBuffer = pSendBuffer->m_pNext;
					}
					pSendBuffer->m_pNext = pBuffer;
				}
			}
			else
			{
				DWORD dwError = NO_ERROR;
				bRet = PostSend(pContext, pBuffer, dwError);
				if (!bRet)
				{
					m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
				}
			}
		}
	}
	return bRet;
#endif
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

bool CIocpTcpServer::SendRequest(SOCKET hSocket, ContextHead* pContextHead, NetRequest* pRequest)
{
	CBuffer objBuffer;
	objBuffer.Write((PBYTE)pContextHead, sizeof(ContextHead));
	objBuffer.Write((PBYTE)pRequest, sizeof(NetRequest));
	objBuffer.Write((PBYTE)pRequest->pData, pRequest->nDataLen);
	return SendData(hSocket, objBuffer.GetBuffer(), objBuffer.GetBufferLen());
}

void CIocpTcpServer::HandleIo(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (!pContext)
	{
		/*���׽������ӽ���ʱ��dwKey = 0 dwTrans = 0 dwError = 0����ʱpContext��ΪNULL�����´��������������*/
		CSocketListenContext* pListen = m_pSocketContextMgr->GetListenCtx();
		if (IoType::enIoAccept == pBuffer->m_ioType && pListen)
		{
			HandleIoAccept(dwKey, pBuffer, dwTrans, dwError);
		}
		else
		{
			m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
		}
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
	case IoType::enIoConn:
		HandleIoConn(dwKey, pBuffer, dwTrans, dwError);
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
		int nLocalLen = sizeof(SOCKADDR_IN);
		int nRemoteLen = sizeof(SOCKADDR_IN);
		pListen->m_lpfnGetAcceptExSockaddrs(
			pBuffer->m_pBuffer->GetData(),
			/*����Accept��ʱ��������������ӵĵ�һ�����ݣ�ͬʱ����ط��ᵼ�»�ȡ��ɶ˿ڶ�������ʱ��Ϊ0*/
			/*pBuffer->m_nBufferLen - ((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			(LPSOCKADDR*)&pContext->m_local,
			&nLocalLen,
			(LPSOCKADDR*)&pContext->m_remote,
			&nRemoteLen
		);

		// ʹ��AcceptEx֮��Ϊ�˵���shutdown����Ҫ�����׽���SO_UPDATE_ACCEPT_CONTEXT(https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex)
		int nRet = 0;
		nRet = setsockopt(pContext->m_hSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&pListen->m_hSocket, sizeof(pListen->m_hSocket));
		if (nRet != 0)
		{
			CloseClient(pContext->m_hSocket);
			m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
			pListen->NotifyRepostAccepts();
			DWORD dwErrCode = ::WSAGetLastError();
			myLogConsoleW("%s �׽���:%d����SO_UPDATE_ACCEPT_CONTEXTѡ��ʧ�� code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
			return;
		}

		/*���½����ӵ��׽���socket����ɶ˿ڰ�*/
		::CreateIoCompletionPort((HANDLE)pContext->m_hSocket, m_hCompletionPort, (DWORD)pContext, 0);

		/*Ϊ������Ͷ��һ��Read����Ͷ��ʧ��ֱ�ӹر�����*/
		CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
		if (pNewBuffer)
		{
			DWORD dwError = 0;
			if (!PostRecv(pContext, pNewBuffer, dwError))
			{
				CloseClient(pContext->m_hSocket);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
				myLogConsoleW("%s �������׽���%dͶ��Read����ʧ�ܣ��Ͽ����� code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwError);
			}
		}
	}
	m_pSocketBufferMgr->RemovePendingAccepts(pBuffer);
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
			CloseClient(pBuffer->m_hSocket);
			myLogConsoleW("%s I/O�����������쳣���ر��׽���%d", __FUNCTION__, pBuffer->m_hSocket);
		}
		else
		{
			DWORD dwError = 0;
			CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
			if (!pNewBuffer || !PostRecv(pContext, pNewBuffer, dwError))
			{
				/*Ͷ�ݶ��¼�ʧ�ܣ��ر�����*/
				CloseClient(pBuffer->m_hSocket);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
				myLogConsoleW("%s I/O�����������쳣���ر��׽���%d code:%d", __FUNCTION__, pBuffer->m_hSocket, dwError);
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
			CloseClient(pBuffer->m_hSocket);
			myLogConsoleI("%s I/Oд���������쳣���ر��׽���%d", __FUNCTION__, pBuffer->m_hSocket);
		}
		else
		{
			bool bClose = false;
			{
				CAutoLock lock(&pContext->m_lock);
				pContext->m_llPendingSends--;
				while (pContext->m_pWaitingSend)
				{
					int nSendLen = 0;
					CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
					if (pNewBuffer)
					{
						while (pContext->m_pWaitingSend && nSendLen < 1024)
						{
							CSocketBuffer* pSendBuffer = pContext->m_pWaitingSend;
							int nDataLen = pSendBuffer->m_pBuffer->GetDataLen();
							if (nSendLen + nDataLen > 1024)
							{
								memcpy(pNewBuffer->m_pBuffer->GetBuffer() + nSendLen, pSendBuffer->m_pBuffer->GetData(), (1024 - nSendLen));
								memmove(pSendBuffer->m_pBuffer->GetData(), (pSendBuffer->m_pBuffer->GetData() + (1024 - nSendLen)), nDataLen - (1024 - nSendLen));
								pSendBuffer->m_pBuffer->SetDataLen(nDataLen - (1024 - nSendLen));
								nSendLen = 1024;
								pNewBuffer->m_pBuffer->SetDataLen(1024);
							}
							else
							{
								memcpy(pNewBuffer->m_pBuffer->GetBuffer() + nSendLen, pSendBuffer->m_pBuffer->GetData(), nDataLen);
								pContext->m_pWaitingSend = pContext->m_pWaitingSend->m_pNext;
								m_pSocketBufferMgr->ReleaseSocketBuffer(pSendBuffer);
								nSendLen += nDataLen;
								pNewBuffer->m_pBuffer->SetDataLen(nSendLen);
							}
						}
						DWORD dwError = NO_ERROR;
						if (!PostSend(pContext, pNewBuffer, dwError))
						{
							myLogConsoleW("%s PostSendʧ�ܣ��ر��׽���%d", __FUNCTION__, pBuffer->m_hSocket);
							bClose = true;
							break;
						}
					}
					else
					{
						myLogConsoleW("%s AllocateSocketBufferʧ�ܣ��׽���%d size:1024", __FUNCTION__, pBuffer->m_hSocket);
						break;
					}
				}
				if (pContext->m_bDelayClose && !pContext->m_pWaitingSend && pContext->m_llPendingSends <= 0)
				{
					bClose = true;
				}
			}
			if (bClose)
			{
#if 1
				CSocketBuffer * pCloseBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
				if (pCloseBuffer)
				{
					DWORD dwError = NO_ERROR;
					if (!PostClose(pContext, pCloseBuffer, dwError))
					{
						m_pSocketBufferMgr->ReleaseSocketBuffer(pCloseBuffer);
					}
				}
#else
				int nRet = NO_ERROR;
				nRet = ::shutdown(pContext->m_hSocket, SD_SEND);
				if (nRet != NO_ERROR)
				{
					int nError = WSAGetLastError();
					myLogConsoleE("%s shutdown failed with errorcode %d", __FUNCTION__, nError);
				}
#endif
			}
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
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		m_pSocketContextMgr->RemovePendingCloses(pBuffer->m_hSocket);
		m_pSocketContextMgr->ReleaseSocketContext(pContext);
	}
	else
	{
		myLogConsoleW("%s pContextΪ��!!!", __FUNCTION__);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::HandleIoConn(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*����Connect����*/
	BOOL bResult = FALSE;
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		int nRet = 0;
		nRet = setsockopt(pContext->m_hSocket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
		if (nRet != 0)
		{
			CloseClient(pContext->m_hSocket);
			DWORD dwErrCode = ::WSAGetLastError();
			myLogConsoleW("%s �׽���:%d����SO_UPDATE_CONNECT_CONTEXTѡ��ʧ�� code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
		}

		CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
		if (pNewBuffer)
		{
			DWORD dwError = 0;
			if (!PostRecv(pContext, pNewBuffer, dwError))
			{
				CloseClient(pContext->m_hSocket);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
				myLogConsoleW("%s �׽���%dͶ��Read����ʧ�ܣ��Ͽ����� code:%d", __FUNCTION__, pContext->m_hSocket, dwError);
			}
		}
	}
	else
	{
		myLogConsoleW("%s pContextΪ��!!!", __FUNCTION__);
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
	NetRequest request = { 0 };
	if (m_nFlag & CPS_FLAG_MSG_HEAD) {
		NetRequest* pRequest = (NetRequest*)pNP->GetData();
		memcpy(&request, pRequest, sizeof(NetRequest));
		request.nDataLen = pNP->GetDataLen() - sizeof(NetRequest);
		request.pData = (PBYTE)pNP->GetData() + sizeof(NetRequest);
	}
	else if (m_nFlag & CPS_FLAG_DEFAULT) {
		request.nDataLen = pNP->GetDataLen();
		request.pData = (void*)pNP->GetData();
	}
	CSocketContext* pContext = pNP->GetCtx();
	OnRequest((void*)pContext, (void*)&request);
	pNP->Release();
}

void CIocpTcpServer::OnRequest(void* p1, void* p2)
{
	__try {

	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}
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
			// ��鵱ǰ��������д��ر����ӣ���ֹ��������ռ���׽�����Դ
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
	while (true)
	{
		if (m_bShutdown)
		{
			myLogConsoleW("%s�����߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}

		/*��ȡ��ɶ˿ڶ�������*/
		CSocketBuffer* pBuffer = NULL;
		DWORD dwKey = 0;
		DWORD dwTrans = 0;
		DWORD dwError = NO_ERROR;
		LPOVERLAPPED lpOverlapped = NULL;
		BOOL bRet = ::GetQueuedCompletionStatus(m_hCompletionPort, &dwTrans, (PULONG_PTR)&dwKey, &lpOverlapped, WSA_INFINITE);

		if (!lpOverlapped)
		{
			if (!bRet)
			{
				dwError = ::WSAGetLastError();
				if (ERROR_ABANDONED_WAIT_0 == dwError)
				{
					myLogConsoleE("%s ERROR_ABANDONED_WAIT_0 %d", __FUNCTION__, ::GetCurrentThreadId());
					break;
				}
			}
			continue;
		}
		/*ȡ����ӦlpOverlapped��CSocketBuffer����I/O����*/
		pBuffer = CONTAINING_RECORD(lpOverlapped, CSocketBuffer, m_ol);
		if (!pBuffer)
		{
			continue;
		}
		if (!bRet)
		{
			/*��ѯ���׽ӿ���һ���ص�����ʧ�ܵ�ԭ��*/
			DWORD dwFlags = 0;
			DWORD cbTransfer = 0;
			BOOL bResult = ::WSAGetOverlappedResult(pBuffer->m_hSocket, &(pBuffer->m_ol), &cbTransfer, FALSE, &dwFlags);
			if (!bResult)
			{
				dwError = ::WSAGetLastError();
				myLogConsoleW("%s WSAGetOverlappedResult����ʧ�ܣ������룺%d", __FUNCTION__, dwError);
			}
			/*�����׽����ϵĴ���*/
			if (NO_ERROR != dwError)
			{
				CloseClient(pBuffer->m_hSocket);
				myLogConsoleW("%s �׽���%d�������� %d", __FUNCTION__, pBuffer->m_hSocket, dwError);
			}
		}

		/*����I/O����*/
		HandleIo(dwKey, pBuffer, dwTrans, dwError);
	}
	myLogConsoleE("%s �����߳�%d�˳�...", __FUNCTION__, ::GetCurrentThreadId());
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

/*CIocpTcpClient*/
CIocpTcpClient::CIocpTcpClient() :
	CIocpTcpServer(CPS_FLAG_MSG_HEAD)
{

}

CIocpTcpClient::~CIocpTcpClient()
{

}

bool CIocpTcpClient::Create()
{
	if (!InitializeIo()) {
		return false;
	}
	if (!InitializeMembers(10)) {
		return false;
	}
	if (!BeginThreadPool()) {
		return false;
	}
	return true;
}

bool CIocpTcpClient::BeginThreadPool(UINT nThreads/* = 0*/)
{
	CThreadContext* pThreadCtx = new CThreadContext();
	pThreadCtx->m_pContext = OnWorkerStart();
	pThreadCtx->m_thread = std::thread([&] {this->WorkerThreadFunc(); });
	std::thread::id tid = pThreadCtx->m_thread.get_id();
	_Thrd_id_t* pid = reinterpret_cast<_Thrd_id_t*>(&tid);
	m_mapWorkerThreadCtx.insert(std::make_pair((DWORD)*pid, pThreadCtx));
	m_socketThread[0] = std::thread([&] {this->SocketThreadFunc(); });
	return true;
}

bool CIocpTcpClient::ConnectOneServer(const std::string strIp, const int nPort, const UINT& nIndex)
{
	if (0 == nIndex)
	{
		myLogConsoleE("%s ConnectOneServerʧ�� index:%d", __FUNCTION__, nIndex);
		return false;
	}
	SOCKET hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hSocket == INVALID_SOCKET)
	{
		myLogConsoleE("%s �����׽���ʧ��", __FUNCTION__);
		return false;
	}
	CSocketContext* pContext = m_pSocketContextMgr->AllocateSocketContext(hSocket, nIndex);
	if (!pContext)
	{
		myLogConsoleE("%s AllocateSocketContextʧ��", __FUNCTION__);
		return false;
	}

	int nRet = 0;
	unsigned long ul = 1;
	nRet = ioctlsocket(pContext->m_hSocket, FIONBIO, (unsigned long*)& ul);
	if (nRet != 0)
	{
		DWORD dwErrCode = ::WSAGetLastError();
		myLogConsoleE("%s �����׽���%dΪ������ģʽʧ�� code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
		return false;
	}

	::CreateIoCompletionPort((HANDLE)pContext->m_hSocket, m_hCompletionPort, (DWORD)pContext, 0);

	SOCKADDR_IN svrAddr;
	memset(&svrAddr, 0, sizeof(SOCKADDR_IN));
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(0);
	svrAddr.sin_addr.s_addr = INADDR_ANY;
	int ret = ::bind(pContext->m_hSocket, (SOCKADDR*)& svrAddr, sizeof(svrAddr));
	if (SOCKET_ERROR == ret)
	{
		DWORD dwErrCode = ::WSAGetLastError();
		myLogConsoleE("%s �׽���%dbindʧ�� code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
		return false;
	}
	svrAddr.sin_port = htons(nPort);
	inet_pton(AF_INET, (PCSTR)strIp.c_str(), &svrAddr.sin_addr);
	memcpy(&pContext->m_remote, &svrAddr, sizeof(SOCKADDR_IN));

	CSocketBuffer* pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
	if (!pBuffer)
	{
		myLogConsoleE("%s AllocateSocketBufferʧ��", __FUNCTION__);
		return false;
	}
	DWORD dwError = NO_ERROR;
	if (!PostConn(pContext, pBuffer, dwError))
	{
		myLogConsoleE("%s Ͷ��IoConnʧ�� �׽���%d code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwError);
		return false;
	}
	return true;
}

bool CIocpTcpClient::BeginConnect(const std::string& strIp, const int& nPort, const UINT& nIndex)
{
	return ConnectOneServer(strIp, nPort, nIndex);
}

bool CIocpTcpClient::DisconnectServer(const UINT& nIndex)
{
	SOCKET hSocket = m_pSocketContextMgr->GetSocket(nIndex);
	if (INVALID_SOCKET == hSocket)
	{
		myLogConsoleE("%s Ŀ�겻���� index:%d", __FUNCTION__, nIndex);
		return false;
	}
	return CloseClient(hSocket);
}

bool CIocpTcpClient::Destroy()
{
	return Shutdown();
}

bool CIocpTcpClient::SendCast(const void* pDataPtr, const int& nDataLen)
{
	std::map<UINT, SOCKET> mapServer;
	m_pSocketContextMgr->GetSockets(mapServer);
	for (auto& iter : mapServer)
	{
		if (iter.second)
		{
			SendData(iter.second, pDataPtr, nDataLen);
		}
	}
	return true;
}

bool CIocpTcpClient::SendOneServer(const UINT& nIndex, const void* pDataPtr, const int& nDataLen)
{
	SOCKET hSocket = m_pSocketContextMgr->GetSocket(nIndex);
	if (INVALID_SOCKET == hSocket)
	{
		myLogConsoleE("%s Ŀ�겻���� index:%d", __FUNCTION__, nIndex);
		return false;
	}
	return SendData(hSocket, pDataPtr, nDataLen);
}

void CIocpTcpClient::OnRequest(void* p1, void* p2)
{
	__try {

	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}
}
