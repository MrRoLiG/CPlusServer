#pragma once

typedef std::vector<LPSOCKET_CONTEXT> CSocketContextVector;
typedef std::vector<SOCKET> CSocketVector;
typedef std::map<SOCKET, PLUSE_PACKAGE> CClientPluseMap;

class CPlusServer{
public:
	CPlusServer();
	~CPlusServer();

public:
	BOOL Initialize();
	BOOL Shutdown();

	BOOL InitializeIocp();

public:
	BOOL OnClientPluse(LPMESSAGE_HEAD lpMessageHead, LPMESSAGE_CONTENT lpMessageContent);

private:
	BOOL PostAccept(LPIO_CONTEXT pIoContext);
	BOOL DoAccept(LPSOCKET_CONTEXT pSocketContext, LPIO_CONTEXT pIoContext);
	BOOL PostSend(LPIO_CONTEXT pIoContext);
	BOOL DoSend(LPSOCKET_CONTEXT pSocketContext, LPIO_CONTEXT pIoContext);
	BOOL PostRecv(LPIO_CONTEXT pIoContext);
	BOOL DoRecv(LPIO_CONTEXT pIoContext);

	BOOL SendRequest(SOCKET sClient, void* pDataPtr, int nDataLen);
	BOOL SendResponse(SOCKET sClient, void* pDataPtr, int nDataLen);
	BOOL OnRequest(void* pParam1, void* pParam2);
	BOOL OnResponse(void* pParam1, void* pParam2);
	
	BOOL HandleErrors(LPSOCKET_CONTEXT pSocketContext, const DWORD& dwErr);
	BOOL HandleMessage(LPIO_CONTEXT pIoContext, int nOpeType);
	void CreateThreads();
	void CloseClients(SOCKET scoSocket);
	void RemoveSocketContext(LPSOCKET_CONTEXT pSocketContext);
	void SerializeNetMessage(CBufferEx& dataBuffer, LPMESSAGE_HEAD lpMessageHead, LPMESSAGE_CONTENT lpMessageContent, void* pData, int nLen);
	void DeserializeNetMessage(LPIO_CONTEXT pIoContext, LPMESSAGE_HEAD pMessageHead, LPMESSAGE_CONTENT pMessageContent);

	inline BOOL FoundClientSocket(SOCKET sSocket);
	inline BOOL FindClientPluse(SOCKET sSocket, PLUSE_PACKAGE& rPlusePackage);

	LPCTSTR GetIniFilePath();
	static unsigned __stdcall WorkerThreadFunc(LPVOID lpParam);
	static unsigned __stdcall DealerThreadFunc(LPVOID lpParam);
	static unsigned __stdcall ClientPluseFunc(LPVOID lpParam);

public:
	TCHAR								m_szIp[IP_LENGTH];							//������IP
	int									m_nPort;									//�������˿�
	unsigned long						m_nTick;									//�������
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