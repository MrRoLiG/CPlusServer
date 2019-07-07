#pragma once

#include <WinSock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")
#include <dbghelp.h > 
#pragma comment(lib, "dbghelp.lib")

#define				SERVER_NAME								"CPlusServer"

#define				SAFE_DELETE(x)							{if(x!=nullptr){delete x;x=nullptr;}}
#define				SAFE_DELETE_ARRAY(x)					{if(x!=nullptr){delete[] x;x=nullptr;}}
#define				SAFE_RELEASE_SOCKET(x)					{if(x!=INVALID_SOCKET){::closesocket(x);x=INVALID_SOCKET;}}
#define				SAFE_RELEASE_HANDLE(x)					{if(x!=NULL&&x!=INVALID_HANDLE_VALUE){::CloseHandle(x);x=INVALID_HANDLE_VALUE;}}

#define				MAX_WORKER_THREAD_NUMBER				32					//��๤���߳���
#define				ADD_ACCEPT_COUNT						32					//AcceptͶ�ݲ���ʱ��ӵ�AcceptͶ����
#define				MAX_WORKER_THREADS_PER_PROCESS			2					//ÿ�����Ķ�Ӧ���������߳�
#define				MAX_MEANWHILE_POST_ACCEPT				1					//ͬʱͶ�ݵ�Accept������
#define				BASE_CBUFFER_SIZE						1024				//�ڴ�ػ������ݻ�������С
#define				BASE_DATA_BUF_SIZE						(1 * 1024)			//�������ݻ�������С
#define				DATA_BUF_SIZE							(8 * 1024)			//���ݻ�������С
#define				ACCEPTEX_BYTES_OFFSET_SIZE				16					//AcceptEx�ֽ�ƫ����
#define				HEART_BEAT_WHEEL_SLOT					60					//�������ʱ���ֲ���
#define				TIME_TICK								1000				//1000ms
#define				IP_LENGTH								16					//IP�ַ�������
#define				MAX_DISCONNECTION_TIMES					3					//���������ȱʧ����
#define				MAX_PLUSE_INTEVAL						30					//���������������

//�Զ����ں���Ϣ
#define				WM_DATA_TO_SEND							(WM_USER + 1000)
#define				WM_DATA_TO_RECV							(WM_USER + 1001)

// iocp�ײ�Э��
#define				BASE_SOCKET_CONNECT						0					// �ͻ�������
#define				BASE_SOCKET_CLOSE						1					// �ͻ��˶Ͽ�
#define				BASE_SOCKET_READ						2					// socket������
#define				BASE_SOCKET_WRITE						3					// socketд����

//����/������
#define				THREAD_EXIT								0					// �߳��˳�

//Э���
#define				PROTOCOL_CLIENT_PLUSE					10000				//����

//��־����ö��
typedef enum enLogLevel {
	enDEFAULT = 0,
	enINFO,
	enDEBUG,
	enWARN,
	enTRACE,
	enERROR,
	enFATAL,
};

//��־�궨��
#if _DEBUG
//�ļ���־��ݺ�
#define		myLogFileI(fmt, ...)		CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileD(fmt, ...)		CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileW(fmt, ...)		CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileT(fmt, ...)		CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileE(fmt, ...)		CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileF(fmt, ...)		CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogFile(fmt, __VA_ARGS__)
//����̨��־��ݺ�
#define		myLogConsoleI(fmt, ...)		CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleD(fmt, ...)		CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleW(fmt, ...)		CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleT(fmt, ...)		CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleE(fmt, ...)		CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleF(fmt, ...)		CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogConsole(fmt, __VA_ARGS__)
#else
//�ļ���־��ݺ�
#define		myLogFileI(fmt, ...)		CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileD(fmt, ...)		CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileW(fmt, ...)		CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileT(fmt, ...)		CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileE(fmt, ...)		CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileF(fmt, ...)		CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogFileEx(fmt, __VA_ARGS__)
#endif

//��ɶ˿���Ͷ��IO��������
typedef enum tagOPE_TYPE{
	OPE_NULL,		//Ĭ��ֵ
	OPE_ACCEPT,		//���տͻ�������
	OPE_RECV,		//��������
	OPE_SEND,		//��������
}OPE_TYPE;

//�������߳�
class CPlusServer;
typedef struct _tagWORKER_THREAD_PARAM{
	CPlusServer*	pServer;
	int				nThreadId;
}WORKER_THREAD_PARAM, *LPWORKER_THREAD_PARAM;

//IO���������ģ�ÿһ���ص������Ĳ�����
typedef struct _tagIO_CONTEXT{
	OVERLAPPED		m_Overlapped;						//�ص������ṹ
	SOCKET			m_Socket;							//IO������ӦSocket
	WSABUF			m_WsaBuf;							//�ص���������������
	CHAR			m_szBuffer[DATA_BUF_SIZE];			//WSABUF������
	OPE_TYPE		m_OpeType;							//��ǰIO��������
}IO_CONTEXT, *LPIO_CONTEXT;

//SOCKET���ݽṹ��
typedef struct _tagSOCKET_CONTEXT{
	SOCKET			m_Socket;							//Socket
	sockaddr_in     m_SockAddrIn;						//��Socket��ַ��Ϣ
	std::vector<LPIO_CONTEXT> m_vectIoContext;			//��Socket���е�IO�����Ĳ���
}SOCKET_CONTEXT, *LPSOCKET_CONTEXT;

class CIoContext{
public:
	CIoContext(){
		ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
		m_Socket = INVALID_SOCKET;
		ZeroMemory(&m_szBuffer, DATA_BUF_SIZE);
		m_WsaBuf.len = sizeof(m_szBuffer);
		m_WsaBuf.buf = m_szBuffer;
		m_OpeType = OPE_NULL;
	}
	~CIoContext(){
		ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
		m_Socket = INVALID_SOCKET;
		ZeroMemory(&m_szBuffer, DATA_BUF_SIZE);
		m_WsaBuf.len = sizeof(m_szBuffer);
		m_WsaBuf.buf = m_szBuffer;
		m_OpeType = OPE_NULL;
	}
public:
	OVERLAPPED	m_Overlapped;
	SOCKET		m_Socket;
	CHAR		m_szBuffer[DATA_BUF_SIZE];
	WSABUF		m_WsaBuf;
	OPE_TYPE	m_OpeType;
};
typedef CIoContext* LPCIoContext;

class CClientContext{
public:
	CClientContext(){
		m_Socket = INVALID_SOCKET;
		ZeroMemory(&m_SockAddrIn, sizeof(sockaddr_in));
		m_vectIoContext.clear();
	}
	~CClientContext(){
		m_Socket = INVALID_SOCKET;
		ZeroMemory(&m_SockAddrIn, sizeof(sockaddr_in));
		m_vectIoContext.clear();
	}
public:
	SOCKET					m_Socket;
	sockaddr_in				m_SockAddrIn;
	std::vector<LPCIoContext> m_vectIoContext;
};
typedef CClientContext* LPClientContext;

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

//��������
typedef struct _tagHEART_BEAT_DETECT{
	SOCKET			m_Socket;							//Socket
	unsigned int	m_uiDisconnectCount;				//������ȱ�մ���
	unsigned int	m_uiLastTick;
}HEART_BEAT_DETECT, *LPHEART_BEAT_DETECT;

typedef struct _tagPLUSE_PACKAGE{
	SOCKET			m_Socket;
	time_t			m_tLastTick;
}PLUSE_PACKAGE, *LPPLUSE_PACKAGE;

///////////////////////////////////////////////////////////////////////////
// �¶�����

// �̲߳���
class CThread;
typedef struct _tagThreadParam {
	bool bSuccess;
	HANDLE hEventHandle;
	CThread* pThread;
}ThreadParam, *LPThreadParam;

// ���ݰ�ͷ
typedef struct _tagDataHead {
	WORD wIdentifier;
	WORD wDataSize;
}DataHead, *LPDataHead;

// ��������������ͷ
typedef struct _tagContextHead{
	SOCKET	hSocket;
	LONG	lToken;
}ContextHead, *LPContextHead;
///////////////////////////////////////////////////////////////////////////