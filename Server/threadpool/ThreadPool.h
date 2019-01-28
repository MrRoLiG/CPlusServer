#pragma once

//----------------------------------------------------------------
//��  ��CEvent
//��;��ʵ���¼�����ķ�װ����Ҫ�����߳�ͬ��
//Ĭ�Ϲ��캯������һ���ֶ����õ��¼�����
//�߳���Ҫ�ȴ�ʱ������Wait()��֪ͨ�߳�ִ��ʱ������Notify()������
//----------------------------------------------------------------
class CEvent
{
public:
	explicit CEvent();			//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CEvent();			//��������

	void Wait();				//�̵߳ȴ�(һֱ�ȴ�)
	int  Wait(int nTimeOut);	//�̵߳ȴ�(nTimeOutʱ���ڵȴ�)
	void Reset();				//�����¼��ź�
	void Notify();				//�����¼��źţ�֪ͨ�߳�ִ��

private:
	HANDLE	m_hEvent;			//�¼�������
};

//CTask��ǰ������
class CTask;

//----------------------------------------------------------------
//�ṹ��  ��priority_queue_cmp
//��;    ������������е��������
//��Ϊpriority_queue�ĵ�����������ʵ���Զ��������������
//----------------------------------------------------------------
template <class T>
struct priority_queue_cmp
{
	bool operator () (const T t1, const T t2) const
	{
		if (t1->GetPriority() > t2->GetPriority())
		{
			return true;
		}

		return false;
	}
};

//�����������ȼ�����������������
typedef std::priority_queue<CTask*, std::vector<CTask*>, priority_queue_cmp<CTask*>> TaskQueue;
//----------------------------------------------------------------
//��  ��CTask
//��;���������
//�����߳��е��õ�����Ϊ��������࣬��Ҫ���ػ����Run()������
//���߳��ڽ������١�
//----------------------------------------------------------------
interface CTask
{
public:
	explicit CTask(int nRriority = 0);	//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CTask();					//��������

	virtual void Run() = 0;				//���麯���ӿڣ���������̳�ʵ��
	virtual void TimeOut();				//��ʱ

	void SetPriority(int nPriority);	//�����������ȼ�
	int GetPriority();					//�õ��������ȼ�
	void SetThreadID(int nThreadID);	//���õ�ǰ�����߳�ID
	int GetThreadID();					//�õ���ǰ�����߳�ID

private:
	int	m_nThreadID;					//ִ�е�ǰ�����߳�ID
	int m_nPriority;					//�������ȼ������ȼ�Խ��Խ��ִ��
};

//----------------------------------------------------------------
//��  ��CTaskQueue
//��;���������
//�����������ȼ�������Զ�������ࡣ
//----------------------------------------------------------------
class CTaskQueue
{
public:
	explicit CTaskQueue();				//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CTaskQueue();				//��������

	CTask* PopTask();					//�Ӷ��е�������
	void PushTask(CTask* pTask);		//�����ѹ������

	void WaitTask();					//�ȴ���������
	void NotifyTask();					//֪ͨ��������

	bool Empty();						//��������Ƿ�Ϊ��
	int Size();							//������е�������

private:
	TaskQueue m_tqTaskQueue;			//�������
	CCritSec m_csTaskQueue;				//�ٽ�������
	CEvent	m_eEvent;					//�¼�����
};

//----------------------------------------------------------------
//ö��  ��enPriority
//��;  ���������ȼ�ö��
//----------------------------------------------------------------
typedef enum enPriority
{
	enLowPriority,		//�����ȼ�
	enMediumPriority,	//�����ȼ�
	enHighPriority,		//�����ȼ�
};

//----------------------------------------------------------------
//ö��  ��enThreadState
//��;  ���߳�����״̬ö��
//----------------------------------------------------------------
typedef enum enThreadState
{
	enInitTS,			//��ʼ״̬
	enSuspendTS,		//����״̬
	enRunningTS,		//����״̬
	enBlockedTS,		//����״̬
	enFinishTS,			//����״̬
};

//CThread��ǰ������
class CThread;

//----------------------------------------------------------------
//��  ��CRunnable
//��;���߳�ִ�е�Ԫ
//ÿ��CThreadͨ������CRunnable�Ĵ���ӿ�Run()ִ�о��������߼���
//----------------------------------------------------------------
class CRunnable
{
public:
	explicit CRunnable(CThread* pThread = NULL);			//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CRunnable();									//��������

	virtual void Run() = 0;									//���麯���ӿڣ���������̳�ʵ��

	void SetThread(CThread* pThread);						//���õ�ǰ�߳�
	CThread* GetThread();									//�õ���ǰ�߳�

	int Wait(int nTimeOut);									//�ȴ��¼�����
	void Notify();											//֪ͨ�¼�����

private:
	CThread* m_pThread;										//��ǰ�߳�
	CEvent m_eEvent;										//�¼�����
};

//----------------------------------------------------------------
//��  ��CThread
//��;���߳���
// C++ �߳���Ļ�����װ��
//----------------------------------------------------------------
class CThread
{
public:
	explicit CThread(CRunnable* pRunnable);					//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CThread();										//��������

	void Start(bool bSuspend = false);						//��ʼ�߳�(Ĭ�ϵ�ǰ�߳�δ����)
	void Excute();											//ִ���߳�
	void Join();											//�ȴ��߳�ִ�����
	void Suspend();											//�����߳�
	void Resume();											//�ָ��߳�
	void Stop();											//ֹͣ�߳�

	void Wait();											//��ʱ�ȴ�
	void Notify();											//֪ͨ�߳�

	void SetThreadState(enThreadState enThreadSte);			//�����߳�״̬
	enThreadState GetThreadState();							//�õ��߳�״̬
	void SetPriority(enPriority enThreadPriority);			//�����߳����ȼ�
	enPriority GetPriority();								//�õ��߳����ȼ�
	void SetThreadID(unsigned int uiThreadID);				//�����߳�ID
	unsigned int GetThreadID();								//�õ��߳�ID

private:
	unsigned int m_uiThreadID;								//�߳�ID
	HANDLE m_hThreadHandle;									//�߳̾��
	CRunnable* m_pRunnable;									//CRunnable����ָ��
	CEvent m_eEvent;										//�¼�����
	enThreadState m_enThreadState;							//�߳�״̬
};

static unsigned int __stdcall ThreadFunc(void* lpParam);//�̺߳���

//CThreadPool��ǰ������
class CThreadPool;

//----------------------------------------------------------------
//��  ��CWorkerThread
//��;�������߳�
//�̳߳������ڸ��������̡߳�
//----------------------------------------------------------------
class CWorkerThread : public CRunnable
{
public:
	explicit CWorkerThread(CThreadPool* pThreadPool = NULL);	//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CWorkerThread();									//��������

	void SetTask(CTask* pTask, void* pData);					//Ϊ��ǰ�����߳�����Task
	CThread* GetThread();										//�õ���ǰ�߳�

	void Run();													//�߳�ִ��
	void Suspend();												//�̹߳���
	void Join();												//�ȴ��߳�ִ�����
	void Resume();												//�ָ��߳�
	void Stop();												//ֹͣ�߳�

private:
	CThreadPool* m_pThreadPool;									//�����̳߳�
	CThread* m_pThread;											//��Ӧ�߳�
	CTask* m_pTask;												//��Ӧ����Task
	CEvent m_eEvent;											//�¼�����

	bool m_bIsExit;												//�߳��Ƿ��˳�
};

//----------------------------------------------------------------
//��  ��CAllocThread
//��;�������߳�
//��������������е��̣߳����������������̡߳�
//----------------------------------------------------------------
class CAllocThread : public CRunnable
{
public:
	explicit CAllocThread(CThreadPool* pThreadPool = NULL);		//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CAllocThread();									//��������

	void Run();													//�߳�ִ��
	void Stop();												//�߳̽���

private:
	CThreadPool* m_pThreadPool;									//�����̳߳�
	CThread* m_pThread;											//��Ӧ�߳�

	bool m_bIsExit;												//�߳��뿪��־
};

//----------------------------------------------------------------
//��  ��CCleanThread
//��;�������߳�
//���������̳߳��ж���Ŀ����̡߳�
//----------------------------------------------------------------
class CCleanThread : public CRunnable
{
public:
	explicit CCleanThread(CThreadPool* pThreadPool = NULL);		//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CCleanThread();									//��������

	void Run();													//�߳�ִ��
	void Stop();												//�߳̽���

private:
	CThreadPool* m_pThreadPool;									//�����̳߳�
	CThread* m_pThread;											//��Ӧ�߳�

	bool m_bIsExit;												//�߳��뿪��־
};

//----------------------------------------------------------------
//��  ��CWorkerThreadList
//��;�������߳��б�
//���̳߳��й����߳��б�ķ�װ��
//----------------------------------------------------------------
class CWorkerThreadList
{
public:
	explicit CWorkerThreadList();								//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CWorkerThreadList();								//��������

	void PushWorkerThread(CWorkerThread* pWorkerThread);		//�������߳�ѹ���б�
	CWorkerThread* PopWorkerThread();							//�������̵߳����б�
	void EraseWorkerThread(CWorkerThread* pWorkerThread);		//ɾ���б��еĹ����߳�
	void Clear();												//�������߳��б�

	void Wait();												//�ȴ�
	int Wait(int nSecs);										//�ȴ�һ��ʱ��
	void Notify();												//֪ͨ

private:
	std::vector<CWorkerThread*> m_vtWorkerThreadList;			//�����߳��б�
	CEvent m_eEvent;											//�¼�����
};

//----------------------------------------------------------------
//��  ��CThreadPool
//��;���̳߳ء�
//----------------------------------------------------------------
class CThreadPool
{
public:
	explicit CThreadPool(int nMaxThreads = 20,
		int nMaxIdleThreads = 10, int nMinIdleThreads = 5);		//���캯����explicit�ؼ������Σ���������������ʽת��
	virtual ~CThreadPool();										//��������

	void Start();												//�����̳߳�
	void Stop();												//�ر��̳߳�

	void CreateWorker(int nWorkerNums);							//���������߳�
	void CleanWorkers();										//�����������߳�
	void MoveToIdleList(CWorkerThread* pWorkerThread);			//��������ɵ��߳��ƻص������߳��б�

	void PushTask(CTask* pTask, void* pData);					//������ѹ���������
	CTask* PopTask();											//������Ӷ����е���

	void HandleTask(CTask* pTask, void* pData);					//��������

	void WaitForTask();											//�̳߳صȴ�����
	void Notify();												//�̳߳ؽ��յ�����

private:
	int m_nMaxThreads;											//�̳߳ع涨����߳���
	int m_nMinIdleThreads;										//���ٿ����߳���
	int m_nMaxIdleThreads;										//�������߳���
	int m_nBusyThreads;											//��ǰæµ�߳���
	int m_nCurrentThreads;										//��ǰ���߳���

	bool m_bIsExit;												//�̳߳��˳���־
	CCritSec m_csMutex;											//�ٽ�������

	CWorkerThreadList m_ltWorkersList;							//�����̱߳�
	CWorkerThreadList m_ltIdleList;								//�����̱߳�

	CTaskQueue m_tqTaskQueue;									//�����������

	CAllocThread* m_pAllcoThread;								//���������߳�
	CCleanThread* m_pCleanThread;								//�����߳�
};