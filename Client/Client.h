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

typedef struct _tagPACKET_HEAD {
	UINT uiPacketNo;
	UINT uiMsgType;
	UINT uiPacketLen;
	BOOL bUseCRC32;
	DWORD dwCRC32;
}PACKET_HEAD, *LPPACKET_HEAD;

class CClient
{
public:
	CClient(char* szServerIP, char* szClientIP, int nPort, int nThreads);
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

	void SendData(SOCKET socket, const char* pData, int nDataLen);
	void SendData(SOCKET socket, UINT nRequest, void* pData, int nDataLen);

	//void PostHttp(std::string data);
	void GetHttp(SOCKET socket);

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