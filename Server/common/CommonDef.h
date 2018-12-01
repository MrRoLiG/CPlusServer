#pragma once

#include <WinSock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")

#define				SAFE_DELETE(x)							{if(x!=NULL){delete x;x=NULL;}}
#define				SAFE_RELEASE_SOCKET(x)					{if(x!=INVALID_SOCKET){closesocket(x);x=INVALID_SOCKET;}}
#define				SAFE_RELEASE_HANDLE(x)					{if(x!=NULL&&x!=INVALID_HANDLE_VALUE){CloseHandle(x);x=NULL;}}

#define				MAX_WORKER_THREADS_PER_PROCESS			2			//ÿ�����Ķ�Ӧ���������߳�
#define				MAX_MEANWHILE_POST_ACCEPT				1			//ͬʱͶ�ݵ�Accept������
#define				BASE_DATA_BUF_SIZE						1 * 1024	//�������ݻ�������С
#define				DATA_BUF_SIZE							2 * 1024	//���ݻ�������С
#define				ACCEPTEX_BYTES_OFFSET_SIZE				16			//AcceptEx�ֽ�ƫ����

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