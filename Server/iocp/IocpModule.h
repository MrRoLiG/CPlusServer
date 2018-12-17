#pragma once

#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")

#include "../common/CommonDef.h"

class CBaseServer;

class CIOCPModule{
public:
	CIOCPModule(CBaseServer* pServer);
	~CIOCPModule();

public:
	BOOL Initialize();
	void DeInitialize();

	static unsigned int __stdcall WorkerThreadFunc(LPVOID lpParam);

	void RemoveSocketContext(LPSOCKET_CONTEXT pSocketContext);

	BOOL HandleErrors(LPSOCKET_CONTEXT pSocketContext, const DWORD& dwErr);

public:
	CBaseServer*					m_pServer;

	HANDLE							m_hIocp;				//��ɶ˿ھ��
	HANDLE*							m_pWorkerThread;		//�����߳�ָ��
	HANDLE							m_hShutdownEvent;		//�߳��˳���־�¼�

	int								m_nWorkerThreads;		//�����߳���
};