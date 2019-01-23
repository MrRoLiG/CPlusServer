#pragma once

class CBaseServer{
public:
	CBaseServer();
	~CBaseServer();

public:
	BOOL Initialize();
	void Shutdown();

	BOOL OnWorkerStart(CIOCPModule* pCIocpModule);
	void OnWorkerExit();

	BOOL LoadSocketLib();
	void UnloadSocketLib();

	BOOL CreateMessageDealerThread();

	BOOL SendRequest(SOCKET client, LPMESSAGE_HEAD lpMessageHead, LPMESSAGE_CONTENT lpMessageContent);
	BOOL SendResponse(SOCKET client, LPMESSAGE_HEAD lpMessageHead, LPMESSAGE_CONTENT lpMessageContent);

	void OnRequest(void* pParam1, void* pParam2);
	void OnResponse(void* pParam1, void* pParam2);

	BOOL OnHeartPluse(LPMESSAGE_HEAD pMessageHead, LPMESSAGE_CONTENT pMessageContent);

	void CloseClients(SOCKET scoSocket);

	LPCTSTR GetIniFileName();
private:
	static unsigned int __stdcall heartbeatFunc(LPVOID pParam);
	static unsigned int __stdcall messageDealerFunc(LPVOID pParam);
	inline void detectHeartBeat(unsigned int& nIndex, HEART_BEAT_DETECT& stuHeartBeatDetect);
	void getIniFile();
	unsigned int getConnections();

	LPSOCKET_CONTEXT getClientSocketContext(SOCKET client);

public:
	TCHAR							m_szIp[IP_LENGTH];							//������IP
	int								m_nPort;									//�������˿�
	unsigned long					m_nTick;									//�������
	TCHAR							m_szIniFilePath[MAX_PATH];					//������ini�����ļ�·��

	CIOCPModule*					m_IocpModule;								//IOCP����
	CIOCPAccept*					m_IocpAccept;								//IOCPAccept����
	CIOCPSocket*					m_IocpSocket;								//IOCPSocket����

	HANDLE							m_hShutdownEvent;							//�������رվ��
	HANDLE							m_hMessageDealerEvent;						//��Ϣ�����¼����
	HANDLE							m_hMessgaeDealerHandle;						//��Ϣ�����߳̾��
	unsigned int					m_uiMessageDealerThreadId;					//��Ϣ�����߳�ID

	CCritSec						m_csVectClientContext;						//vector���ٽ�������
	std::vector<LPSOCKET_CONTEXT>	m_vectClientConetxt;						//�洢��ǰ�������ӿͻ��˵�Context��Ϣ
	CCritSec							m_csMapClientHeartBeat;					//���ӿͻ���Map����ٽ�������
	std::map<SOCKET, HEART_BEAT_DETECT> m_mapClientHeartBeat;					//�洢��ǰ�������ӿͻ��˵����������Ϣ
	CCritSec						m_csHeartBeatWheel;							//����������ٽ�������
	std::vector<SOCKET>				m_vectHeartBeatWheel[HEART_BEAT_WHEEL_SLOT];//���������

	LPSOCKET_CONTEXT				m_pListenContext;							//�����ͻ��˵�Context��Ϣ

	LPFN_ACCEPTEX					m_pFnAcceptEx;								//AcceptEx�ĺ���ָ��
	LPFN_GETACCEPTEXSOCKADDRS		m_pFnGetAcceptExSockAddr;					//GetAcceptExSockAddr�ĺ���ָ��
};