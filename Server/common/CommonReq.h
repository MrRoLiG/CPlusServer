#pragma once

#define TEST_CONNECTION		10000

//��־����ö��
typedef enum enLogLevel{
	enDEFAULT = 0,
	enINFO,
	enDEBUG,
	enWARN,
	enTRACE,
	enERROR,
	enFATAL,
};
//�ļ���־��ݺ�
#if	_DEBUG
#define		myLogFileI(fmt, ...)		CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileD(fmt, ...)		CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileW(fmt, ...)		CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileT(fmt, ...)		CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileE(fmt, ...)		CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogFile(fmt, __VA_ARGS__)
#define		myLogFileF(fmt, ...)		CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogFile(fmt, __VA_ARGS__)
#else
#define		myLogFileI(fmt, ...)		CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileD(fmt, ...)		CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileW(fmt, ...)		CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileT(fmt, ...)		CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileE(fmt, ...)		CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogFileEx(fmt, __VA_ARGS__)
#define		myLogFileF(fmt, ...)		CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogFileEx(fmt, __VA_ARGS__)
#endif
//����̨��־��ݺ�
#define		myLogConsoleI(fmt, ...)		CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleD(fmt, ...)		CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleW(fmt, ...)		CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleT(fmt, ...)		CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleE(fmt, ...)		CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogConsole(fmt, __VA_ARGS__)
#define		myLogConsoleF(fmt, ...)		CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogConsole(fmt, __VA_ARGS__)

// CBufferPool��Ϣ
typedef struct _tagBUFFER_INFO {
	DWORD dwDataSize;
	DWORD dwBufferSize;
	DWORD dwDataPacketCount;
}BUFFER_INFO, *LPBUFFER_INFO;