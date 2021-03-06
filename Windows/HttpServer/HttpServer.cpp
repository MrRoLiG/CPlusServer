#include <iostream>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include "HttpServer.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

std::string GetOSName()
{
	std::string osName = ("");
	int a = 0, b = 0, i = 0, j = 0;
	_asm
	{
		pushad
		mov ebx, fs: [0x18] ; get self pointer from TEB
		mov eax, fs: [0x30] ; get pointer to PEB / database
		mov ebx, [eax + 0A8h]; get OSMinorVersion
		mov eax, [eax + 0A4h]; get OSMajorVersion
		mov j, ebx
		mov i, eax
		popad
	}

	if ((i == 5) && (j == 0))
	{
		osName = ("2000");
	}
	else if ((i == 5) && (j == 1))
	{
		osName = ("XP");
	}
	else if ((i == 5) && (j == 2))
	{
		osName = ("2003");
	}
	else if ((i == 6) && (j == 0))
	{
		osName = ("Vista");
	}
	else if ((i == 6) && (j == 1))
	{
		osName = ("7");
	}
	else if ((i == 6) && (j == 2))
	{
		osName = ("8");
	}
	else if ((i == 6) && (j == 3))
	{
		osName = ("8.1");
	}
	else if ((i == 10) && (j == 0))
	{
		osName = ("10");
	}
	else
	{
		osName = ("unknow");
	}
	return osName;
}

void DisableSetUnhandledExceptionFilter()
{
	void* addr = (void*)::GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter");
	if (addr)
	{
		unsigned char code[16];
		int size = 0;
		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		::VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		::WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		::VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

void WriteMiniDMP(struct _EXCEPTION_POINTERS* pExp)
{
	CHAR szFilePathTp[MAX_PATH];
	TCHAR wszFilePath[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), wszFilePath, MAX_PATH - 1);
	int nLen = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wszFilePath, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wszFilePath, -1, szFilePathTp, nLen, NULL, NULL);
	CHAR* pPtr = strrchr(szFilePathTp, '\\');

	CHAR szDumpFile[MAX_PATH];
	string strStr = szFilePathTp;
	size_t nIndex = strStr.find(pPtr);
	string strRes = strStr.substr(0, nIndex);
	sprintf_s(szDumpFile, "%s\\%lld.dmp", strRes.c_str(), _time64(nullptr));

	TCHAR wszDumpFile[MAX_PATH];
	int wnLen = MultiByteToWideChar(CP_ACP, NULL, szDumpFile, strlen(szDumpFile), NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, szDumpFile, strlen(szDumpFile) + 1, wszDumpFile, wnLen + 1);
	HANDLE hFile = ::CreateFile(wszDumpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExp;
		ExInfo.ClientPointers = NULL;
		BOOL bOK = ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo, NULL, NULL);
		::CloseHandle(hFile);
	}
}

LONG WINAPI ExpFilter(struct _EXCEPTION_POINTERS* pExp)
{
	WriteMiniDMP(pExp);
	return EXCEPTION_EXECUTE_HANDLER;
}

void CDBClient::OnRequest(void* p1, void* p2) {
	CSocketContext* pContext = (CSocketContext*)p1;
	NetRequest* pRequest = (NetRequest*)p2;
	ContextHead* pContextHead = (ContextHead*)((PBYTE)pRequest->pData);
	char* pRet = (char*)((PBYTE)pRequest->pData + pRequest->head.nRepeated * sizeof(ContextHead));
	std::string strRet = std::string(pRet);
	m_pServer->SendData(pContextHead->hSocket, strRet.c_str(), strRet.size());
	m_pServer->CloseClient(pContextHead->hSocket);
}

int main()
{
	::SetUnhandledExceptionFilter(ExpFilter);
	if ("10" != GetOSName())
	{
		// ����CRT�쳣
		DisableSetUnhandledExceptionFilter();
	}
	CHttpServer* pHttpServer = new CHttpServer();
	if (!pHttpServer || !pHttpServer->Initialize("127.0.0.1", 8080, 32, 64, 4, 20480))
	{
		myLogConsoleI("server initialize failed");
	}
	char g = getchar();
	while ('q' == g)
	{
		pHttpServer->Shutdown();
		break;
	}
	_CrtDumpMemoryLeaks();
	return 0;
}
