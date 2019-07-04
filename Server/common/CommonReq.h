#pragma once

#define TEST_CONNECTION		10000
/************************************************************************/
/* Utilties.h                                                           */
/*CBufferPool��Ϣ*/
typedef struct _tagBUFFER_INFO {
	DWORD dwDataSize;
	DWORD dwBufferSize;
	DWORD dwDataPacketCount;
}BUFFER_INFO, *LPBUFFER_INFO;
/************************************************************************/
/************************************************************************/
/* iocp.h                                                               */
/*���ݰ�ͷ*/
typedef struct _tagPACKET_HEAD{
	UINT uiPacketNo;
	UINT uiMsgType;
	UINT uiPacketLen;
	BOOL bUseCRC32;
	DWORD dwCRC32;
}PACKET_HEAD, *LPPACKET_HEAD;
/*�׽���������ͷ��Ϣ*/
typedef struct _tagCONTEXT_HEAD{
	SOCKET hSocket;
	UINT uiTokenID;
	UINT uiSessionID;
}CONTEXT_HEAD, *LPCONTEXT_HEAD;
/*����ͷ�ļ�*/
typedef struct _tagREQUEST_HEAD{
	UINT nRepeated;
	UINT nRequest;
}REQUEST_HEAD, *LPREQUEST_HEAD;
/************************************************************************/