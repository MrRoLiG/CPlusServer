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

private:
	static unsigned int __stdcall _heartbeatFunc(LPVOID pParam);
	inline void _detectHeartBeat(unsigned int nUserID, unsigned int& nIndex, HEART_BEAT_DETECT& stuHeartBeatDetect);
public:
	TCHAR* _getIniFile();

public:
	CHAR*							m_szIp;						//������IP
	int								m_nPort;					//�������˿�

	CIOCPModule*					m_IocpModule;				//IOCP����
	CIOCPAccept*					m_IocpAccept;				//IOCPAccept����
	CIOCPSocket*					m_IocpSocket;				//IOCPSocket����

	HANDLE							m_hShutdownEvent;			//�������رվ��

	CritSec							m_csVectClientContext;		//vector���ٽ�������
	std::vector<LPSOCKET_CONTEXT>	m_vectClientConetxt;		//�洢��ǰ�������ӿͻ��˵�Context��Ϣ

	CritSec							m_csMapSocket;				//���ӿͻ���Map����ٽ�������
	std::map<unsigned int, SOCKET>	m_mapClientSocket;			//�洢��ǰ�������ӿͻ��˵�Socket��Ϣ

	CritSec							m_csMapClientHeartBeat;		//���ӿͻ���Map����ٽ�������
	std::map<SOCKET, HEART_BEAT_DETECT> m_mapClientHeartBeat;		//�洢��ǰ�������ӿͻ��˵����������Ϣ

	unsigned long					m_nTick;					//�������
	CritSec							m_csHeartBeatWheel;			//����������ٽ�������
	std::vector<SOCKET>				m_listHeartBeatWheel[HEART_BEAT_WHEEL_SLOT];
																//���������

	LPSOCKET_CONTEXT				m_pListenContext;			//�����ͻ��˵�Context��Ϣ

	LPFN_ACCEPTEX					m_pFnAcceptEx;				//AcceptEx�ĺ���ָ��
	LPFN_GETACCEPTEXSOCKADDRS		m_pFnGetAcceptExSockAddr;	//GetAcceptExSockAddr�ĺ���ָ��
};