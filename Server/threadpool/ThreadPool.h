#pragma once
#include <process.h>

// �Զ����߳̽ӿ���
interface IThread
{
	// ״̬�ж�
	virtual bool IsRunning() = 0;
	// �����߳�
	virtual bool StartThread() = 0;
	// ֹͣ�߳�
	virtual bool StopThread(DWORD dwWaitSeconds) = 0;
	// ��ֹ�߳�
	virtual bool TerminateThread(DWORD dwExitCode) = 0;
};
// �Զ����߳���
class CThread : public IThread
{
	// ��������
public:
	CThread();
	virtual ~CThread();

	virtual bool IsRunning();
	virtual bool StartThread();
	virtual bool StopThread(DWORD dwWaitSeconds = INFINITE);
	virtual bool TerminateThread(DWORD dwExitCode);

	UINT GetThreadID() { return m_uiThreadID; }
	HANDLE GetThreadHandle() { return m_hThreadHandle; }

private:
	virtual bool Run() = 0;
	static unsigned __stdcall ThreadFunction(LPVOID lpParam);

private:
	volatile bool	m_bRun;
	UINT			m_uiThreadID;
	HANDLE			m_hThreadHandle;
};