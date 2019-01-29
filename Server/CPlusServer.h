#pragma once

typedef std::vector<LPSOCKET_CONTEXT> CSocketContextVector;
typedef std::vector<SOCKET> CSocketVector;
typedef std::map<SOCKET, PLUSE_PACKAGE> CClientPluseMap;

class CPlusServer{
public:
	CPlusServer();
	~CPlusServer();

public:
	virtual BOOL Initialize();
	virtual BOOL Shutdown();

	virtual BOOL InitializeIocp();

public:
	virtual BOOL OnClientPluse(LPMESSAGE_HEAD lpMessageHead, LPMESSAGE_CONTENT lpMessageContent);

private:
	virtual BOOL PostAccept(LPIO_CONTEXT pIoContext);
	virtual BOOL DoAccept(LPSOCKET_CONTEXT pSocketContext, LPIO_CONTEXT pIoContext);
	virtual BOOL PostSend(LPIO_CONTEXT pIoContext);
	virtual BOOL DoSend(LPSOCKET_CONTEXT pSocketContext, LPIO_CONTEXT pIoContext);
	virtual BOOL PostRecv(LPIO_CONTEXT pIoContext);
	virtual BOOL DoRecv(LPIO_CONTEXT pIoContext);

	virtual BOOL SendRequest(SOCKET sClient, int nRequest, void* pDataPtr, int nDataLen);
	virtual BOOL SendResponse(SOCKET sClient, int nRequest, void* pDataPtr, int nDataLen);
	virtual BOOL OnRequest(void* pParam1, void* pParam2);
	virtual BOOL OnResponse(void* pParam1, void* pParam2);
	virtual BOOL SendData(SOCKET sClient, void* pDataPtr, int nDataLen);
	virtual BOOL SimulateRequest(int nRequest, void* pDataPtr, int nDataLen);
	
	virtual BOOL HandleNetMessage(LPIO_CONTEXT pIoContext, int nOpeType);
	virtual void CreateThreads();
	virtual void CloseClients(SOCKET scoSocket);
	virtual void RemoveSocketContext(LPSOCKET_CONTEXT pSocketContext);
	virtual void SerializeNetMessage(CBufferEx& dataBuffer, LPMESSAGE_HEAD lpMessageHead, LPMESSAGE_CONTENT lpMessageContent, void* pData, int nLen);
	virtual void DeserializeNetMessage(LPIO_CONTEXT pIoContext, LPMESSAGE_HEAD pMessageHead, LPMESSAGE_CONTENT pMessageContent);

	virtual inline BOOL FindClientSocket(SOCKET sSocket, SOCKET_CONTEXT& rSocketContext);
	virtual inline BOOL FindClientPluse(SOCKET sSocket, PLUSE_PACKAGE& rPlusePackage);

	LPCTSTR GetIniFilePath();
	static unsigned __stdcall WorkerThreadFunc(LPVOID lpParam);
	static unsigned __stdcall DealerThreadFunc(LPVOID lpParam);
	static unsigned __stdcall ClientPluseFunc(LPVOID lpParam);

public:
	TCHAR								m_szIp[IP_LENGTH];							//������IP
	int									m_nPort;									//�������˿�
	TCHAR								m_szIniFilePath[MAX_PATH];					//������ini�����ļ�·��

	HANDLE								m_hIocp;									//��ɶ˿ھ��
	HANDLE								m_hShutdownEvent;							//�������رվ��

	int									m_nWorkerThreads;							//�����߳���
	HANDLE*								m_phWorkerThread;							//�����߳̾��ָ��

	HANDLE								m_hClientPluseHandle;						//��������߳̾��
	unsigned int						m_uiClientPluseThreadId;					//��������߳�ID

	HANDLE								m_hMessageDealerEvent;						//��Ϣ�����¼����
	HANDLE								m_hMessgaeDealerHandle;						//��Ϣ�����߳̾��
	unsigned int						m_uiMessageDealerThreadId;					//��Ϣ�����߳�ID

	CCritSec							m_csVectClientContext;						//vector���ٽ�������
	CSocketContextVector				m_vectClientConetxt;						//�洢��ǰ�������ӿͻ��˵�Context��Ϣ

	CCritSec							m_csMapClientPluse;							//����map���ٽ�������
	CClientPluseMap						m_mapClientPluse;							//����map

	LPSOCKET_CONTEXT					m_pListenContext;							//�����ͻ��˵�Context��Ϣ

	LPFN_ACCEPTEX						m_pFnAcceptEx;								//AcceptEx�ĺ���ָ��
	LPFN_GETACCEPTEXSOCKADDRS			m_pFnGetAcceptExSockAddr;					//GetAcceptExSockAddr�ĺ���ָ��
};