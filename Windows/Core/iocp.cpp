#include "iocp.h"

/*CSocketContext*/
bool CSocketContext::HandleRecvData(CSocketBuffer* pBuffer, DWORD dwTrans, CIocpTcpServer* pServer)
{
	if (dwTrans <= 0)
	{
		myLogConsoleW("%s 套接字%d断开连接", __FUNCTION__, pBuffer->m_hSocket);
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
			/*pBuffer应该插在表头*/
			pBuffer->m_pNext = m_pWaitingRecv;
			m_pWaitingRecv = pBuffer;
		}
		else
		{
			/*pBuffer应该插在表中*/
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
	myLogConsoleI("%s 服务关闭", __FUNCTION__);

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
		myLogConsoleE("%s 套接字bind错误", __FUNCTION__);
		::closesocket(m_pSocketContextMgr->GetListenSocket());
		::WSACleanup();
		return false;
	}

	nRet = ::listen(m_pSocketContextMgr->GetListenSocket(), SOMAXCONN);
	if (SOCKET_ERROR == nRet)
	{
		myLogConsoleE("%s 套接字listen错误", __FUNCTION__);
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
		myLogConsoleE("%s m_hCompletionPort句柄无效", __FUNCTION__);
		return false;
	}

	return true;
}

bool CIocpTcpServer::Initialize(const char* lpSzIp, UINT nPort, UINT nInitAccepts, UINT nMaxAccpets, UINT nThreads, UINT nMaxConnections)
{
	if (!InitializeIo())
	{
		myLogConsoleE("%s InitializeIo失败", __FUNCTION__);
		return false;
	}
	if (!InitializeMembers(nMaxConnections))
	{
		myLogConsoleE("%s InitializeMembers失败", __FUNCTION__);
		return false;
	}
	if (!BeginBindListen(lpSzIp, nPort, nInitAccepts, nMaxAccpets))
	{
		myLogConsoleE("%s BeginListen失败", __FUNCTION__);
		return false;
	}
	if (!BeginThreadPool(nThreads))
	{
		myLogConsoleE("%s BeginThreadPool失败", __FUNCTION__);
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

	// 设置为非阻塞
	unsigned long ul = 1;
	int nRet = ioctlsocket(pBuffer->m_hSocket, FIONBIO, (unsigned long*)& ul);
	if (nRet != 0)
	{
		return false;
	}

	// acceptEx长度设定为1024（完成数据包大小）再加上本机和远端SOCKADDR_IN长度
	DWORD dwBytes = 0;
	BYTE acceptEx[1024 + 2 * (sizeof(SOCKADDR_IN) + 16)];
	ZeroMemory(&acceptEx, 1024 + 2 * (sizeof(SOCKADDR_IN) + 16));
	BOOL bRet = pListen->m_lpfnAcceptEx(
		pListen->m_hSocket,
		pBuffer->m_hSocket,
		&acceptEx,
		/*不在Accept的时候接收来自新连接的第一份数据，同时这个地方会导致获取完成端口队列数据时均为0*/
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
		myLogConsoleW("%s WSARecv 失败", __FUNCTION__);
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
		myLogConsoleW("%s 套接字%d正在关闭", __FUNCTION__, pContext->m_hSocket);
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
		myLogConsoleW("%s WSASend 失败 WSAGetLastError:%ld", __FUNCTION__, dwWSAError);
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
		myLogConsoleE("%s 获取ConnectEx函数指针失败", __FUNCTION__);
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
		myLogConsoleW("%s 套接字%d正在关闭", __FUNCTION__, pContext->m_hSocket);
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
		myLogConsoleE("%s 获取DisconnectEx函数指针失败", __FUNCTION__);
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
		myLogConsoleE("%s DisconnectEx失败 %d WSAGetLastError:%ld", __FUNCTION__, pContext->m_hSocket, dwError);
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
		myLogConsoleW("%s 发生的pb数据过多", __FUNCTION__);
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
		/*新套接字连接建立时，dwKey = 0 dwTrans = 0 dwError = 0，此时pContext会为NULL，更新待处理的连接链表*/
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

	/*分别处理I/O操作*/
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
	// 暂时不做处理
}

void CIocpTcpServer::HandleIoAccept(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Accept请求*/
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
			/*不在Accept的时候接收来自新连接的第一份数据，同时这个地方会导致获取完成端口队列数据时均为0*/
			/*pBuffer->m_nBufferLen - ((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			(LPSOCKADDR*)&pContext->m_local,
			&nLocalLen,
			(LPSOCKADDR*)&pContext->m_remote,
			&nRemoteLen
		);

		// 使用AcceptEx之后，为了调用shutdown，需要设置套接字SO_UPDATE_ACCEPT_CONTEXT(https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex)
		int nRet = 0;
		nRet = setsockopt(pContext->m_hSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&pListen->m_hSocket, sizeof(pListen->m_hSocket));
		if (nRet != 0)
		{
			CloseClient(pContext->m_hSocket);
			m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
			pListen->NotifyRepostAccepts();
			DWORD dwErrCode = ::WSAGetLastError();
			myLogConsoleW("%s 套接字:%d设置SO_UPDATE_ACCEPT_CONTEXT选项失败 code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
			return;
		}

		/*将新建连接的套接字socket和完成端口绑定*/
		::CreateIoCompletionPort((HANDLE)pContext->m_hSocket, m_hCompletionPort, (DWORD)pContext, 0);

		/*为新连接投递一个Read请求，投递失败直接关闭连接*/
		CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
		if (pNewBuffer)
		{
			DWORD dwError = 0;
			if (!PostRecv(pContext, pNewBuffer, dwError))
			{
				CloseClient(pContext->m_hSocket);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
				myLogConsoleW("%s 新连接套接字%d投递Read请求失败，断开连接 code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwError);
			}
		}
	}
	m_pSocketBufferMgr->RemovePendingAccepts(pBuffer);
	/*Accept I/O操作完成，释放pBuffer*/
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
	/*通知Accept线程中的m_hRepostHandle事件重新投递一个Accept操作*/
	pListen->NotifyRepostAccepts();
}

void CIocpTcpServer::HandleIoRead(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Read请求*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		bool bRet = pContext->HandleRecvData(pBuffer, dwTrans, this);
		if (!bRet)
		{
			/*连接处理接受数据失败，关闭连接*/
			CloseClient(pBuffer->m_hSocket);
			myLogConsoleW("%s I/O读操作出现异常，关闭套接字%d", __FUNCTION__, pBuffer->m_hSocket);
		}
		else
		{
			DWORD dwError = 0;
			CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
			if (!pNewBuffer || !PostRecv(pContext, pNewBuffer, dwError))
			{
				/*投递读事件失败，关闭连接*/
				CloseClient(pBuffer->m_hSocket);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
				myLogConsoleW("%s I/O读操作出现异常，关闭套接字%d code:%d", __FUNCTION__, pBuffer->m_hSocket, dwError);
			}
		}
	}
	else
	{
		myLogConsoleW("%s pContext为空!!!", __FUNCTION__);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::HandleIoWrite(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Write请求*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		if (0 == dwTrans)
		{
			/*I/O写操作出现异常，关闭该套接字的连接，释放相关资源*/
			CloseClient(pBuffer->m_hSocket);
			myLogConsoleI("%s I/O写操作出现异常，关闭套接字%d", __FUNCTION__, pBuffer->m_hSocket);
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
							myLogConsoleW("%s PostSend失败，关闭套接字%d", __FUNCTION__, pBuffer->m_hSocket);
							bClose = true;
							break;
						}
					}
					else
					{
						myLogConsoleW("%s AllocateSocketBuffer失败，套接字%d size:1024", __FUNCTION__, pBuffer->m_hSocket);
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
		myLogConsoleW("%s pContext为空!!!", __FUNCTION__);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::HandleIoClose(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Close请求*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		m_pSocketContextMgr->RemovePendingCloses(pBuffer->m_hSocket);
		m_pSocketContextMgr->ReleaseSocketContext(pContext);
	}
	else
	{
		myLogConsoleW("%s pContext为空!!!", __FUNCTION__);
	}
	m_pSocketBufferMgr->ReleaseSocketBuffer(pBuffer);
}

void CIocpTcpServer::HandleIoConn(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Connect请求*/
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
			myLogConsoleW("%s 套接字:%d设置SO_UPDATE_CONNECT_CONTEXT选项失败 code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
		}

		CSocketBuffer* pNewBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
		if (pNewBuffer)
		{
			DWORD dwError = 0;
			if (!PostRecv(pContext, pNewBuffer, dwError))
			{
				CloseClient(pContext->m_hSocket);
				m_pSocketBufferMgr->ReleaseSocketBuffer(pNewBuffer);
				myLogConsoleW("%s 套接字%d投递Read请求失败，断开连接 code:%d", __FUNCTION__, pContext->m_hSocket, dwError);
			}
		}
	}
	else
	{
		myLogConsoleW("%s pContext为空!!!", __FUNCTION__);
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
		myLogConsoleW("%s ListenContext is nullptr, Accept线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
		return;
	}

	CSocketBuffer* pBuffer = nullptr;
	DWORD dwError = 0;

	for (UINT i = 0; i < m_nInitAccepts; i++)
	{
		pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
		if (!pBuffer)
		{
			myLogConsoleW("%s 线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
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
			myLogConsoleW("%s Accept线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}

		// 定时对m_hWaitHandle进行监听，在预留的已经投递AcceptEx句柄即将用完之前及时添加，监听失败直接返回
		DWORD dwWaitRet = ::WSAWaitForMultipleEvents(nEventCount, m_hWaitHandle, FALSE, 5000, FALSE);
		if (WSA_WAIT_FAILED == dwWaitRet)
		{
			myLogConsoleW("%s Accept线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
			Shutdown();
			return;
		}
		else if (WSA_WAIT_TIMEOUT == dwWaitRet)
		{
			// 检查当前挂起的所有AcceptEx I/O建立时长
			m_pSocketBufferMgr->CheckPendingAccpets();
			// 检查当前挂起的所有待关闭连接，防止恶意连接占用套接字资源
			m_pSocketContextMgr->CheckPendingCloses();
		}
		else if(WSA_WAIT_EVENT_0 == dwWaitRet)
		{
			// 查询Accept事件处理：dwWaitRet返回值区间位于[WSA_WAIT_EVENT_0， (WSA_WAIT_EVENT_0+ nEventCount - 1)]，对应索引，超出代表发生错误
			dwWaitRet = dwWaitRet - WSA_WAIT_EVENT_0;
			int nAddAcceptCounts = 0;
			if (0 == dwWaitRet)
			{
				WSANETWORKEVENTS wsaNetEvent;
				::WSAEnumNetworkEvents(pListenCtx->m_hSocket, m_hWaitHandle[dwWaitRet], &wsaNetEvent);
				if (FD_ACCEPT & wsaNetEvent.lNetworkEvents)
				{
					// 发送FD_ACCEPT网络事件，意味着投递的Accept不够，需要增加
					nAddAcceptCounts = 64;
				}
			}
			else if (1 == dwWaitRet)
			{
				nAddAcceptCounts = ::InterlockedExchange(&pListenCtx->m_nRepostCount, 0);;
			}
			else if (dwWaitRet >= (DWORD)nEventCount)
			{
				// 网络事件索引超出，发生错误，关闭服务
				Shutdown();
				myLogConsoleI("%s Accept线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
				return;
			}

			// 增加Accept投递
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
	myLogConsoleI("%s Accept线程%d退出...", __FUNCTION__, GetCurrentThreadId());
	return;
}

void CIocpTcpServer::SocketThreadFunc()
{
	while (true)
	{
		if (m_bShutdown)
		{
			myLogConsoleW("%s工作线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
			return;
		}

		/*获取完成端口队列数据*/
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
		/*取出对应lpOverlapped的CSocketBuffer进行I/O操作*/
		pBuffer = CONTAINING_RECORD(lpOverlapped, CSocketBuffer, m_ol);
		if (!pBuffer)
		{
			continue;
		}
		if (!bRet)
		{
			/*查询该套接口上一个重叠操作失败的原因*/
			DWORD dwFlags = 0;
			DWORD cbTransfer = 0;
			BOOL bResult = ::WSAGetOverlappedResult(pBuffer->m_hSocket, &(pBuffer->m_ol), &cbTransfer, FALSE, &dwFlags);
			if (!bResult)
			{
				dwError = ::WSAGetLastError();
				myLogConsoleW("%s WSAGetOverlappedResult返回失败，错误码：%d", __FUNCTION__, dwError);
			}
			/*检查该套接字上的错误*/
			if (NO_ERROR != dwError)
			{
				CloseClient(pBuffer->m_hSocket);
				myLogConsoleW("%s 套接字%d发生错误 %d", __FUNCTION__, pBuffer->m_hSocket, dwError);
			}
		}

		/*处理I/O操作*/
		HandleIo(dwKey, pBuffer, dwTrans, dwError);
	}
	myLogConsoleE("%s 工作线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
	return;
}

void CIocpTcpServer::WorkerThreadFunc()
{
	while (true)
	{
		std::deque<NetPacket> diptPacket;
		if (m_bShutdown)
		{
			myLogConsoleW("%s WorkerThreadFunc线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
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
	myLogConsoleW("%s WorkerThreadFunc线程%d退出...", __FUNCTION__, ::GetCurrentThreadId());
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
		myLogConsoleE("%s ConnectOneServer失败 index:%d", __FUNCTION__, nIndex);
		return false;
	}
	SOCKET hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hSocket == INVALID_SOCKET)
	{
		myLogConsoleE("%s 创建套接字失败", __FUNCTION__);
		return false;
	}
	CSocketContext* pContext = m_pSocketContextMgr->AllocateSocketContext(hSocket, nIndex);
	if (!pContext)
	{
		myLogConsoleE("%s AllocateSocketContext失败", __FUNCTION__);
		return false;
	}

	int nRet = 0;
	unsigned long ul = 1;
	nRet = ioctlsocket(pContext->m_hSocket, FIONBIO, (unsigned long*)& ul);
	if (nRet != 0)
	{
		DWORD dwErrCode = ::WSAGetLastError();
		myLogConsoleE("%s 设置套接字%d为非阻塞模式失败 code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
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
		myLogConsoleE("%s 套接字%dbind失败 code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwErrCode);
		return false;
	}
	svrAddr.sin_port = htons(nPort);
	inet_pton(AF_INET, (PCSTR)strIp.c_str(), &svrAddr.sin_addr);
	memcpy(&pContext->m_remote, &svrAddr, sizeof(SOCKADDR_IN));

	CSocketBuffer* pBuffer = m_pSocketBufferMgr->AllocateSocketBuffer(1024);
	if (!pBuffer)
	{
		myLogConsoleE("%s AllocateSocketBuffer失败", __FUNCTION__);
		return false;
	}
	DWORD dwError = NO_ERROR;
	if (!PostConn(pContext, pBuffer, dwError))
	{
		myLogConsoleE("%s 投递IoConn失败 套接字%d code:%d", __FUNCTION__, pContext->m_hSocket, (int)dwError);
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
		myLogConsoleE("%s 目标不存在 index:%d", __FUNCTION__, nIndex);
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
		myLogConsoleE("%s 目标不存在 index:%d", __FUNCTION__, nIndex);
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
