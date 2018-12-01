#pragma once

class CBaseServer{
public:
	CBaseServer();
	~CBaseServer();

public:
	BOOL Initialize();
	void Shutdown();

	BOOL OnWorkerStart(CIOCPModule* pCIocModule);
	void OnWorkerExit();

	BOOL LoadSocketLib();
	void UnloadSocketLib();

	void TestSend();

public:
	CHAR*							m_szIp;						//������IP
	int								m_nPort;					//�������˿�

	CIOCPModule*					m_IocpModule;				//IOCP����
	CIOCPAccept*					m_IocpAccept;				//IOCPAccept����
	CIOCPSocket*					m_IocpSocket;				//IOCPSocket����

	HANDLE							m_hShutdownEvent;			//�������رվ��

	CritSec							m_csVectClientContext;		//vector���ٽ�������
	std::vector<LPSOCKET_CONTEXT>	m_vectClientConetxt;		//�洢��ǰ�������ӿͻ��˵�Context��Ϣ

	LPSOCKET_CONTEXT				m_pListenContext;			//�����ͻ��˵�Context��Ϣ

	LPFN_ACCEPTEX					m_pFnAcceptEx;				//AcceptEx�ĺ���ָ��
	LPFN_GETACCEPTEXSOCKADDRS		m_pFnGetAcceptExSockAddr;	//GetAcceptExSockAddr�ĺ���ָ��
};