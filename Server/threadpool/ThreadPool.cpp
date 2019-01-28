#include "stdafx.h"

CEvent::CEvent()
{
	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	if (INVALID_HANDLE_VALUE == m_hEvent)
	{
		CONSOLE_ERROR("�¼�����ʧ��!");
	}
}
CEvent::~CEvent()
{
	if (INVALID_HANDLE_VALUE != m_hEvent)
	{
		CloseHandle(m_hEvent);
	}
	m_hEvent = INVALID_HANDLE_VALUE;
}
void CEvent::Wait()
{
	if (WAIT_FAILED == WaitForSingleObject(m_hEvent, INFINITE))
	{
		CONSOLE_ERROR("�¼��ȴ�ʧ��!");
	}
}
int CEvent::Wait(int nTimeOut)
{
	int nRet = 0;
	DWORD dwMillicSec = 0;
	DWORD dwStatus = 0;

	dwMillicSec = nTimeOut * 1000;
	dwStatus = WaitForSingleObject(m_hEvent, dwMillicSec);
	if (WAIT_TIMEOUT == dwStatus)
	{
		CONSOLE_ERROR("�¼��ȴ���ʱ!");
	}
	if (WAIT_FAILED == dwStatus)
	{
		CONSOLE_ERROR("�¼��ȴ�ʧ��!");
	}

	return nRet;
}
void CEvent::Reset()
{
	if (!ResetEvent(m_hEvent))
	{
		CONSOLE_ERROR("�¼��ź�����ʧ��!");
	}
}
void CEvent::Notify()
{
	if (!SetEvent(m_hEvent))
	{
		CONSOLE_ERROR("�¼��ź�����ʧ��!");
	}
}

CTask::CTask(int nRriority /* = 0 */)
{
	m_nPriority = nRriority;
	m_nThreadID = -1;
}
CTask::~CTask()
{

}
void CTask::TimeOut()
{
	CONSOLE_INFOS("û���̴߳���ǰ���񣬳�ʱ!");
}
void CTask::SetPriority(int nPriority)
{
	m_nPriority = nPriority;
}
int CTask::GetPriority()
{
	return m_nPriority;
}
void CTask::SetThreadID(int nThreadID)
{
	m_nThreadID = nThreadID;
}
int CTask::GetThreadID()
{
	return m_nThreadID;
}

CTaskQueue::CTaskQueue()
{

}
CTaskQueue::~CTaskQueue()
{

}
CTask* CTaskQueue::PopTask()
{
	CTask* pTask = NULL;

	CAutoLock lock(&m_csTaskQueue);
	if (!m_tqTaskQueue.empty())
	{
		pTask = dynamic_cast<CTask*>(m_tqTaskQueue.top());
		m_tqTaskQueue.pop();
	}

	return pTask;
}
void CTaskQueue::PushTask(CTask* pTask)
{
	CAutoLock lock(&m_csTaskQueue);
	m_tqTaskQueue.push(pTask);

	NotifyTask();
}
void CTaskQueue::WaitTask()
{
	m_eEvent.Wait();
}
void CTaskQueue::NotifyTask()
{
	m_eEvent.Notify();
}
bool CTaskQueue::Empty()
{
	CAutoLock lock(&m_csTaskQueue);
	return m_tqTaskQueue.empty();
}
int CTaskQueue::Size()
{
	CAutoLock lock(&m_csTaskQueue);
	return m_tqTaskQueue.size();
}

CRunnable::CRunnable(CThread* pThread /* = NULL */)
:m_pThread(pThread)
{

}
CRunnable::~CRunnable()
{

}
void CRunnable::SetThread(CThread* pThread)
{
	m_pThread = pThread;
}
CThread* CRunnable::GetThread()
{
	return m_pThread;
}
int CRunnable::Wait(int nTimeOut)
{
	return m_eEvent.Wait(nTimeOut);
}
void CRunnable::Notify()
{
	m_eEvent.Notify();
}


CThread::CThread(CRunnable* pRunnable)
{
	m_pRunnable = pRunnable;
	m_uiThreadID = -1;
	m_hThreadHandle = INVALID_HANDLE_VALUE;
	m_enThreadState = enInitTS;

	pRunnable->SetThread(this);
}
CThread::~CThread()
{
	if (m_enThreadState != enFinishTS)
	{
		if (m_enThreadState == enSuspendTS)
		{
			Resume();
			Join();
		}
		if (m_enThreadState == enBlockedTS)
		{
			Notify();
			Join();
		}
		if (m_enThreadState == enRunningTS)
		{
			Join();
		}
	}
	if (INVALID_HANDLE_VALUE != m_hThreadHandle)
	{
		CloseHandle(m_hThreadHandle);
	}

	m_hThreadHandle = INVALID_HANDLE_VALUE;
	m_uiThreadID = -1;
}
unsigned int __stdcall ThreadFunc(void* lpParam)
{
	CThread* pThread = (CThread*)lpParam;

	pThread->Wait();
	pThread->Excute();
	pThread->Stop();

	return 0;
}
void CThread::Start(bool bSuspend /* = false */)
{
	if (bSuspend)
	{
		m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, CREATE_SUSPENDED, &m_uiThreadID);
	}
	else
	{
		m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &m_uiThreadID);
	}

	if (INVALID_HANDLE_VALUE == m_hThreadHandle)
	{
		m_enThreadState = enFinishTS;

	}
}
void CThread::Excute()
{
	m_enThreadState = enRunningTS;
	m_pRunnable->Run();
}
void CThread::Join()
{
	WaitForSingleObject(m_hThreadHandle, INFINITE);
	m_enThreadState = enFinishTS;
}
void CThread::Suspend()
{
	if (m_enThreadState == enSuspendTS || m_enThreadState == enBlockedTS)
	{
		return;
	}

	SuspendThread(m_hThreadHandle);
	m_enThreadState = enSuspendTS;
}
void CThread::Resume()
{
	if (m_enThreadState == enSuspendTS)
	{
		ResumeThread(m_hThreadHandle);
		m_enThreadState = enRunningTS;
	}
}
void CThread::Stop()
{
	m_enThreadState = enFinishTS;
}
void CThread::Wait()
{
	m_enThreadState = enBlockedTS;
	m_eEvent.Wait();
	m_enThreadState = enRunningTS;
}
void CThread::Notify()
{
	m_eEvent.Notify();
}
void CThread::SetThreadState(enThreadState enThreadSte)
{
	m_enThreadState = enThreadSte;
}
enThreadState CThread::GetThreadState()
{
	return m_enThreadState;
}
void CThread::SetPriority(enPriority enThreadPriority)
{
	SetThreadPriority(m_hThreadHandle, enThreadPriority);
}
enPriority CThread::GetPriority()
{
	return (enPriority)GetThreadPriority(m_hThreadHandle);
}
void CThread::SetThreadID(unsigned int uiThreadID)
{
	m_uiThreadID = uiThreadID;
}
unsigned int CThread::GetThreadID()
{
	return m_uiThreadID;
}

CWorkerThread::CWorkerThread(CThreadPool* pThreadPool /* = NULL */)
{
	m_pThreadPool = pThreadPool;
	m_pThread = new CThread(this);
	m_pThread->Start();
	m_pThread->Notify();

	m_pTask = NULL;
	m_bIsExit = false;
}
CWorkerThread::~CWorkerThread()
{
	if (m_pThread)
	{
		delete m_pThread;
	}
	m_pThread = NULL;
}
void CWorkerThread::SetTask(CTask* pTask, void* pData)
{
	m_pTask = pTask;
	m_pTask->SetThreadID(GetThread()->GetThreadID());
	//�������߳��趨����֮��֪ͨCWorkerThread::Run()��ʼִ������
	m_eEvent.Notify();
}
CThread* CWorkerThread::GetThread()
{
	return m_pThread;
}
void CWorkerThread::Run()
{
	while (true)
	{
		//�ڹ����̴߳���ʱ���ȵȴ�����
		m_eEvent.Wait();

		if (m_bIsExit)
		{
			break;
		}

		m_pTask->Run();
		//����ִ����ɣ�����
		if (m_pTask)
		{
			delete m_pTask;
			m_pTask = NULL;
		}

		//��������֪ͨ�̳߳ص�ǰ�߳��Ѿ�����
		m_pThreadPool->MoveToIdleList(this);
		//�ֶ������ź�
		m_eEvent.Reset();
	}
}
void CWorkerThread::Suspend()
{
	if (m_pThread)
	{
		m_pThread->Suspend();
	}
}
void CWorkerThread::Join()
{
	if (m_pThread)
	{
		m_pThread->Join();
	}
}
void CWorkerThread::Resume()
{
	if (m_pThread)
	{
		m_pThread->Resume();
	}
}
void CWorkerThread::Stop()
{
	//����Stop()ʱ��֪ͨCWorkerThread::Run()��ǰ�߳���Ҫֹͣ���˳��߳�
	m_bIsExit = true;
	m_eEvent.Notify();
}


CAllocThread::CAllocThread(CThreadPool* pThreadPool /* = NULL */)
{
	m_pThreadPool = pThreadPool;
	m_pThread = new CThread(this);
	m_pThread->Start();
	//֪ͨ�߳̿�ʼ����CThread::Excute()
	m_pThread->Notify();

	m_bIsExit = false;
}
CAllocThread::~CAllocThread()
{
	if (m_pThread)
	{
		delete m_pThread;
	}
	m_pThread = NULL;
}
void CAllocThread::Run()
{
	while (true)
	{
		if (m_bIsExit)
		{
			break;
		}

		CTask* pTask = m_pThreadPool->PopTask();
		if (pTask)
		{
			m_pThreadPool->HandleTask(pTask, NULL);
		}
		else
		{
			m_pThreadPool->WaitForTask();
		}
	}
}
void CAllocThread::Stop()
{
	m_bIsExit = true;
	m_pThreadPool->Notify();
}

CCleanThread::CCleanThread(CThreadPool* pThreadPool /* = NULL */)
{
	m_pThreadPool = pThreadPool;
	m_pThread = new CThread(this);
	m_pThread->Start();
	//֪ͨ�߳̿�ʼ����CThread::Excute()
	m_pThread->Notify();

	m_bIsExit = false;
}
CCleanThread::~CCleanThread()
{
	if (m_pThread)
	{
		delete m_pThread;
	}
	m_pThread = NULL;
}
void CCleanThread::Run()
{
	while (true)
	{
		if (m_bIsExit)
		{
			break;
		}

		m_pThreadPool->CleanWorkers();
	}
}
void CCleanThread::Stop()
{
	m_bIsExit = true;
	Notify();
}

CWorkerThreadList::CWorkerThreadList()
{

}
CWorkerThreadList::~CWorkerThreadList()
{

}
void CWorkerThreadList::PushWorkerThread(CWorkerThread* pWorkerThread)
{
	if (pWorkerThread)
	{
		m_vtWorkerThreadList.push_back(pWorkerThread);
	}
}
CWorkerThread* CWorkerThreadList::PopWorkerThread()
{
	CWorkerThread* pWorkerThread = NULL;
	if (!m_vtWorkerThreadList.empty())
	{
		pWorkerThread = m_vtWorkerThreadList.front();
		m_vtWorkerThreadList.erase(m_vtWorkerThreadList.begin());
	}

	return pWorkerThread;
}
void CWorkerThreadList::EraseWorkerThread(CWorkerThread* pWorkerThread)
{
	if (pWorkerThread)
	{
		std::vector<CWorkerThread*>::iterator iter;
		iter = std::find(m_vtWorkerThreadList.begin(), m_vtWorkerThreadList.end(), pWorkerThread);
		if (iter != m_vtWorkerThreadList.end())
		{
			m_vtWorkerThreadList.erase(iter);
		}
	}
}
void CWorkerThreadList::Clear()
{
	m_vtWorkerThreadList.clear();
}
void CWorkerThreadList::Wait()
{
	m_eEvent.Wait();
}
int CWorkerThreadList::Wait(int nSecs)
{
	return m_eEvent.Wait(nSecs);
}
void CWorkerThreadList::Notify()
{
	m_eEvent.Notify();
}

CThreadPool::CThreadPool(int nMaxThreads /* = 20 */, int nMaxIdleThreads /* = 10 */, int nMinIdleThreads /* = 5 */)
{
	m_nMaxThreads = nMaxThreads;
	m_nMaxIdleThreads = nMaxIdleThreads;
	m_nMinIdleThreads = nMinIdleThreads;

	m_ltWorkersList.Clear();
	m_ltIdleList.Clear();
}
CThreadPool::~CThreadPool()
{
	if (m_pAllcoThread)
	{
		delete m_pAllcoThread;
	}
	m_pAllcoThread = NULL;

	if (m_pCleanThread)
	{
		delete m_pCleanThread;
	}
	m_pCleanThread = NULL;
}
void CThreadPool::Start()
{
	m_pAllcoThread = new CAllocThread(this);
	m_pCleanThread = new CCleanThread(this);

	m_nBusyThreads = 0;
	m_nCurrentThreads = 0;

	CreateWorker(m_nMinIdleThreads);

	m_bIsExit = false;
}
void CThreadPool::Stop()
{
	//�̳߳�ֹͣ
	if (m_bIsExit)
	{
		m_pAllcoThread->Stop();
		m_pCleanThread->Stop();

		if (m_pAllcoThread)
		{
			delete m_pAllcoThread;
		}
		m_pAllcoThread = NULL;

		if (m_pCleanThread)
		{
			delete m_pCleanThread;
		}
		m_pCleanThread = NULL;

		while (true)
		{
			CAutoLock lock(&m_csMutex);
			CWorkerThread* pWorkerThread = m_ltWorkersList.PopWorkerThread();
			if (!pWorkerThread)
			{
				break;
			}
			pWorkerThread->Stop();
			delete pWorkerThread;
			pWorkerThread = NULL;
		}

		m_ltIdleList.Clear();
		m_ltWorkersList.Clear();

		m_nBusyThreads = 0;
		m_nCurrentThreads = 0;

		CTask* pTask = NULL;
		while (true)
		{
			pTask = m_tqTaskQueue.PopTask();
			if (pTask)
			{
				pTask->TimeOut();
				delete pTask;
			}
			else
			{
				break;
			}
		}
		pTask = NULL;
	}
}
void CThreadPool::CreateWorker(int nWorkerNums)
{
	CAutoLock lock(&m_csMutex);
	if (nWorkerNums > m_nMaxThreads)
	{
		nWorkerNums = m_nMaxThreads;
	}

	for (auto i = 0; i < nWorkerNums; i++)
	{
		CWorkerThread* pWorkerThread = new CWorkerThread(this);
		m_ltWorkersList.PushWorkerThread(pWorkerThread);
		m_ltIdleList.PushWorkerThread(pWorkerThread);

		m_nCurrentThreads++;
		CONSOLE_INFOS("����һ���߳�,��ǰ�߳���%d", m_nCurrentThreads);
	}
}
void CThreadPool::CleanWorkers()
{
	int nWorkersToFree = m_nCurrentThreads - m_nBusyThreads;
	if (nWorkersToFree < m_nMaxIdleThreads)
	{
		return;
	}

	int nFreeNum = nWorkersToFree - m_nMaxIdleThreads;
	for (auto i = 0; i < nFreeNum; i++)
	{
		CAutoLock lock(&m_csMutex);
		CWorkerThread* pWorkerThread = m_ltIdleList.PopWorkerThread();
		if (pWorkerThread)
		{
			pWorkerThread->Stop();
			pWorkerThread->Join();
			m_ltIdleList.EraseWorkerThread(pWorkerThread);
			delete pWorkerThread;
			pWorkerThread = NULL;

			m_nCurrentThreads--;
		}
	}
}
void CThreadPool::MoveToIdleList(CWorkerThread* pWorkerThread)
{
	CAutoLock lock(&m_csMutex);
	m_ltIdleList.PushWorkerThread(pWorkerThread);
	m_nBusyThreads--;

	m_ltIdleList.Notify();
}
void CThreadPool::PushTask(CTask* pTask, void* pData)
{
	m_tqTaskQueue.PushTask(pTask);
}
CTask* CThreadPool::PopTask()
{
	return m_tqTaskQueue.PopTask();
}
void CThreadPool::HandleTask(CTask* pTask, void* pData)
{
	if (0 == m_nCurrentThreads)
	{
		CONSOLE_INFOS("��ǰ�̳߳����߳���Ϊ0,ȷ����ȷ�����̳߳�!");
	}
	if (pTask)
	{
		//��ǰ�̶߳���æµʱ��������Ӷ����߳�
		if (m_nBusyThreads == m_nCurrentThreads)
		{
			if (m_nCurrentThreads < m_nMaxThreads)
			{
				CAutoLock lock(&m_csMutex);
				CONSOLE_INFOS("��ǰ�̳߳�æµ,��Ҫ��Ӷ����߳�!");
				int nAllow = m_nMaxThreads - m_nCurrentThreads;
				int nWorkersToCreate = nAllow > m_nMinIdleThreads ? m_nMinIdleThreads : nAllow;
				CreateWorker(nWorkersToCreate);
			}
			else
			{
				//��ǰ�̳߳��Ѿ��ﵽ����߳�������ʼ�ȴ������߳�
				CONSOLE_INFOS("��ǰ�̳߳��Ѿ��ﵽ����߳���,���ڵȴ������߳�!");
				//�������һֱ�ȴ��Ļ�����������������߳̿����ʱ��Ӧ�ý���֪ͨ��
				m_ltIdleList.Wait();
			}
		}

		CAutoLock lock(&m_csMutex);
		CWorkerThread* pWorkerThread = m_ltIdleList.PopWorkerThread();
		if (pWorkerThread)
		{
			//Ϊ��ǰ�����߳�ָ������
			pWorkerThread->SetTask(pTask, pData);
			m_nBusyThreads++;
		}
	}
}
void CThreadPool::WaitForTask()
{
	m_tqTaskQueue.WaitTask();
}
void CThreadPool::Notify()
{
	m_tqTaskQueue.NotifyTask();
}