#pragma once

#include <WinSock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")

#define				SAFE_DELETE(x)							{if(x!=NULL){delete x;x=NULL;}}
#define				SAFE_RELEASE_SOCKET(x)					{if(x!=INVALID_SOCKET){closesocket(x);x=INVALID_SOCKET;}}
#define				SAFE_RELEASE_HANDLE(x)					{if(x!=NULL&&x!=INVALID_HANDLE_VALUE){CloseHandle(x);x=NULL;}}

#define				MAX_WORKER_THREADS_PER_PROCESS			2			//ÿ�����Ķ�Ӧ���������߳�
#define				MAX_MEANWHILE_POST_ACCEPT				1			//ͬʱͶ�ݵ�Accept������
#define				BASE_DATA_BUF_SIZE						(1 * 1024)	//�������ݻ�������С
#define				DATA_BUF_SIZE							(8 * 1024)	//���ݻ�������С
#define				ACCEPTEX_BYTES_OFFSET_SIZE				16			//AcceptEx�ֽ�ƫ����
#define				HEART_BEAT_WHEEL_SLOT					60			//�������ʱ���ֲ���
#define				TIME_TICK								1000		//1000ms
#define				IP_LENGTH								16			//IP�ַ�������
#define				MAX_DISCONNECTION_TIMES					3			//���������ȱʧ����

//Э���
#define				PROTOCOL_HEART_PLUSE					10000		//����

//��ɶ˿���Ͷ��IO��������
typedef enum tagOPE_TYPE{
	OPE_NULL,		//Ĭ��ֵ
	OPE_ACCEPT,		//���տͻ�������
	OPE_RECV,		//��������
	OPE_SEND,		//��������
}OPE_TYPE;

//�������߳�
class CIOCPModule;
typedef struct _tagWORKER_THREAD{
	CIOCPModule*	pCIOCPModule;
	int				nThreadId;
}WORKER_THREAD, *LPWORKER_THREAD;

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