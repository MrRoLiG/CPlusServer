// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

void TestTimer(void* pParam)
{
	std::cout << "Hello World!" << std::endl;
}
void CancelTest(void* pParam)
{
	std::cout << "Canceled!" << std::endl;
}

class testTask : public CTask
{
public:
	testTask(int nPriority = 0, int nTest = 0) :CTask(nPriority)
	{
		m_nTest = nTest;
	}
	~testTask()
	{

	}

	virtual void Run();

private:
	int m_nTest;
};
void testTask::Run()
{
	printf("ִ������%d\n", m_nTest);
}

int _tmain(int argc, _TCHAR* argv[])
{
#if 1
	CBaseServer GameServer = CBaseServer();
	//GetPrivateProfileString(_T("Server"), _T("ip"), _T(""), GameServer.m_szIp, _countof(GameServer.m_szIp), GameServer._getIniFile());
	if (FALSE == GameServer.Initialize())
	{
		//LOG_ERROR("Iniatialize Failed...");
	}
	char ch;
	do
	{
		ch = 'A';
		ch = toupper(ch);

		if ('S' == ch)
		{
			//GameServer.TestSend();
		}
		//ch = 'Q';

	} while ('Q' != ch);
#elif 0	//������־���
	
#elif 0	//�����ڴ��
	char test[] = "test";
	MESSAGE_HEAD stuMessageHead = { 0 };
	stuMessageHead.hSocket = 100;
	stuMessageHead.lSession = 100;
	stuMessageHead.lTokenID = 100;
	MESSAGE_CONTENT stuMessageContent = { 0 };
	stuMessageContent.nRequest = 10000;
	stuMessageContent.nDataLen = sizeof(test);
	stuMessageContent.pDataPtr = test;
	/*CBuffer myDataPool;
	myDataPool.Write((PBYTE)&stuMessageHead, sizeof(MESSAGE_HEAD));
	myDataPool.Write((PBYTE)&stuMessageContent, sizeof(MESSAGE_CONTENT));
	LPMESSAGE_HEAD lpHead = LPMESSAGE_HEAD(PBYTE(myDataPool.GetBuffer()));
	LPMESSAGE_CONTENT lpContent = LPMESSAGE_CONTENT(PBYTE(myDataPool.GetBuffer()+sizeof(MESSAGE_HEAD)));*/
	
	/*CBufferEx myDataPool;
	myDataPool.Write((PBYTE)"Test", sizeof("Test"));
	myDataPool.Write((PBYTE)"Test", sizeof("Test"));
	std::cout<<myDataPool.c_Bytes() + sizeof("Test")<<std::endl;*/
	CBufferEx myDataPool;
	myDataPool.Write((PBYTE)&stuMessageHead, sizeof(MESSAGE_HEAD));
	myDataPool.Write((PBYTE)&stuMessageContent, sizeof(MESSAGE_CONTENT));
	LPMESSAGE_HEAD lpHead = LPMESSAGE_HEAD(PBYTE(myDataPool.c_Bytes()));
	LPMESSAGE_CONTENT lpContent = LPMESSAGE_CONTENT(PBYTE(myDataPool.c_Bytes() + sizeof(MESSAGE_HEAD)));

	getchar();
#elif 0	//����Redis����
	CRedisManager Manager("127.0.0.1",6379,0);
	Manager.ConnectServer("127.0.0.1", 6379, "8767626", 0);
	Manager.PingServer();
#elif 0	//����MySQL����
	_ConnectionPtr connection = CMySQLManager::GetInstance()->GetConnection();
	//MYSQL_TestConnection(connection);
	//DB_TestMySQL(connection,"select id from test;");
	//����ִ��д���ݲ���ʱ�����Իع�
	auto nResult = MYSQL_TestConnection(connection);
	if(nResult < 0)
	{
		printf("database access failed...\n");
		return 0;
	}
	//������ݿ�����
	nResult = MYSQL_BeginTrans(connection);
	if (nResult <= 0)
	{
		printf("database access failed...\n");
	}
	nResult = DB_TestMySQL(connection, "insert into test values(999,\'insert\')");
	if (nResult <= 0)
	{
		printf("action invalid...rollback...\n");
		//����ع�
		MYSQL_RollbackTrans(connection);
		return 0;
	}
	//�ύ�������
	MYSQL_CommitTrans(connection);
#elif 0	//�����̳߳�
	CThreadPool* pThreadPool = new CThreadPool(20, 10, 5);
	pThreadPool->Start();

	for (auto i = 0; i < 20; i++)
	{
		testTask* test = new testTask(0, i);
		pThreadPool->PushTask(test, NULL);
	}

	pThreadPool->Stop();
#elif 0	//���Զ�ʱ��
	TimeWheel timer;
	timer.SetTimer(10000, TestTimer, NULL, CancelTest);
	//TimeWheelNode* pNode = timer.SetTimer(10000, TestTimer, NULL, CancelTest);
	//timer.CancelTimer(pNode->nTimerID);
#else
	cout << "Nothing Is Done!" << endl;
#endif
	getchar();

	return 0;
}