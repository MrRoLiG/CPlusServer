#pragma once

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_IP		"127.0.0.1"
#define DEFAULT_PORT	8888
#define DEFAULT_THREAD	1000

#define MAC_TEST_SEND		10000
#define MAC_TEST_SEND_EX	10001

#define MAX_DATA_BUF_SIZE	(8 * 1024)

class CClient;

typedef struct _stuWORKER_THREAD_PARAM
{
	CClient* pClient;
	SOCKET sSocket;
	int nThreadId;
	char szBuffer[MAX_DATA_BUF_SIZE];
}WORKER_THREAD_PARAM, *LPWORKER_THREAD_PARAM;

typedef struct _stuREQUEST_HEAD {
	UINT nRequest;
}REQUEST_HEAD, *LPREQUEST_HEAD;

typedef struct _stuBASE_DATA {
	int stuRequestHead;
	CHAR dataBuff[MAX_DATA_BUF_SIZE];
}BASE_DATA, *LPBASE_DATA;

typedef struct _tagMESSAGE_HEAD{
	SOCKET	hSocket;
	LONG	lSession;
	LONG	lTokenID;
}MESSAGE_HEAD, *LPMESSAGE_HEAD;

typedef struct _tagMESSAGE_CONTENT{
	UINT	nRequest;
	int		nDataLen;
	void*	pDataPtr;
}MESSAGE_CONTENT, *LPMESSAGE_CONTENT;

class CClient
{
public:
	CClient();
	~CClient();

public:
	BOOL Run();
	void Stop();
	void CleanUp();

public:
	BOOL LoadSocketLib();
	void UnloadSocketLib();

private:
	BOOL InitialiazeConnection();
	BOOL ConnectToServer(SOCKET* pSocket, char* pServerIP, int nPort);

	void SendData(SOCKET socket, char* pData, int nDataLen);
	void SendData(SOCKET socket, UINT nRequest, void* pData, int nDataLen);

	static unsigned int __stdcall ConnectionThread(LPVOID lpParam);
	static unsigned int __stdcall SendThread(LPVOID lpParam);
	static unsigned int __stdcall RecvThread(LPVOID lpParam);

private:
	char*						m_szServerIP;				//������IP
	char*						m_szClientIP;				//�ͻ���IP
	int							m_nPort;					//�ͻ���Port
	int							m_nThreads;					//�ͻ���ָ���߳���

	HANDLE*						m_phSendThreads;			//���������߳���ָ��
	HANDLE*						m_phRecvThreads;			//���������߳���ָ��
	WORKER_THREAD_PARAM*		m_pWorkerThreadParam;		//�̲߳���

	HANDLE						m_hConnectionThread;		//���������Ӿ��
	HANDLE						m_hShutdownEvent;			//�ͻ��˹ر����Ӿ��
	CRITICAL_SECTION			m_csSend;
};