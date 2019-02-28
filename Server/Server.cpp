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

class testTask : public CTask{
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
	CONSOLE_INFOS("ִ������%d", m_nTest);
}

BOOL testConsume(LPMQ_MESSAGE pMessage)
{
	CONSOLE_INFOS("consumed message with message id:%d ", pMessage->nMessageID);
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
#if 0	//����������
	CBaseServer GameServer = CBaseServer();
	if (FALSE == GameServer.Initialize())
	{
		FILE_ERROR("%s iniatialize failed...", __FUNCTION__);
		CONSOLE_ERROR("%s iniatialize failed...", __FUNCTION__);
	}
	char ch;
	do
	{
		ch = 'A';
		ch = toupper(ch);

	} while ('Q' != ch);
	GameServer.Shutdown();
#elif 0
	CPlusServer GameServer = CPlusServer();
	if (FALSE == GameServer.Initialize())
	{
		FILE_ERROR("%s gameserver iniatialize failed...", __FUNCTION__);
		CONSOLE_ERROR("%s gameserver iniatialize failed...", __FUNCTION__);
	}
	char ch;
	do
	{
		ch = 'A';
		ch = toupper(ch);
	} while ('Q' != ch);
	GameServer.Shutdown();
#elif 0	//�����ڴ����
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
#elif 0 //���Ի��λ�����
	char writechar[8] = { 0 };
	memcpy(writechar, "hello", sizeof(writechar));
	CRingBuffer Buffer(16);
	Buffer.Write(writechar, sizeof(writechar));
	char readchar[4] = { 0 };
	CONSOLE_INFOS("the length is %d", Buffer.GetLength());
	Buffer.Read(readchar, sizeof(readchar));
	CONSOLE_INFOS("read data:%s ",readchar);
	char readchar1[8] = { 0 };
	Buffer.Read(readchar1, sizeof(readchar1));
	CONSOLE_INFOS("read data:%s ",readchar);
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
#elif 0 //������Ϣ����MQ_Manager
	MQ_Manager* pManager = new MQ_Manager;
	MQ_Publisher* pPublisher = new MQ_Publisher;
	MQ_Subscriber* pSubscriber1 = new MQ_Subscriber;
	MQ_Subscriber* pSubscriber2 = new MQ_Subscriber;

	MQ_MESSAGE stuMQMessage = { 0 };
	stuMQMessage.nMessageID = 1000;
	stuMQMessage.nMessageType = 1;
	strcpy(stuMQMessage.szMessageContent, "hello world!");

	pPublisher->PublishMQ(pManager, stuMQMessage.nMessageID, &stuMQMessage);
	pSubscriber1->SubscribMQ(pManager, stuMQMessage.nMessageID);
	pSubscriber2->SubscribMQ(pManager, stuMQMessage.nMessageID);
	pSubscriber2->UnSubscribMQ(pManager, stuMQMessage.nMessageID);

	MQ_MESSAGE stuMQMessage1 = { 0 };
	stuMQMessage1.nMessageID = 2000;
	stuMQMessage1.nMessageType = 2;
	strcpy(stuMQMessage1.szMessageContent, "hello world! hello world!");
	pPublisher->PublishMQ(pManager, stuMQMessage1.nMessageID, &stuMQMessage1);
	pSubscriber2->SubscribMQ(pManager, stuMQMessage1.nMessageID);

	//delete pManager;
	//delete pPublisher;
	//delete pSubscriber1;
	//delete pSubscriber2;
#elif 0 //������Ϣ����MQ_ManagerEx
	MQ_MESSAGE stuMQMessage = { 0 };
	stuMQMessage.nMessageID = 1000;
	stuMQMessage.nMessageType = 1;
	strcpy(stuMQMessage.szMessageContent, "hello world!");
	while (1)
	{
		CMessageQueueEx::GetInstance()->SetCallbackFunc(testConsume)->Produce(&stuMQMessage);
		CONSOLE_INFOS("produced message with message id:%d ", stuMQMessage.nMessageID);
		stuMQMessage.nMessageID++;
	}
	
	//delete pManager;
#elif 0	//�����Զ�����־
	CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogFile("INFO %d", 1);
	FILE_INFOS("INFO %d", 1);
	FILE_INFOS("INFO %d", 2);
	FILE_INFOS("INFO %d", 3);
	FILE_INFOS("INFO %d", 4);
	LOG_INFO_FILE("INFO %d %s", 5, "hello world!");
	CLog::GetInstance()->SetLogLevel(enDEBUG)->WriteLogFile("DEBUG %d", 2);
	LOG_DEBUG_FILE("DEBUG %d %s", 1, "DEBUG");
	CLog::GetInstance()->SetLogLevel(enWARN)->WriteLogFile("WARN %d", 3);
	CLog::GetInstance()->SetLogLevel(enTRACE)->WriteLogFile("TRACE %d", 4);
	CLog::GetInstance()->SetLogLevel(enERROR)->WriteLogFile("ERROR %d", 5);
	CLog::GetInstance()->SetLogLevel(enFATAL)->WriteLogFile("FATAL %d", 6);
	CLog::GetInstance()->SetLogLevel(enINFO)->WriteLogFileEx("test %d", 1);
#elif 0 //����JsonCpp
	std::string str = "{\"string\": \"string\",\"int\": 100,\"bool\": \"True\"}";
	char* strStudent = "{ \"name\" : \"cuishihao\", \"age\" : 28, \"major\" : \"cs\" }";
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(str, root))
	{
		std::string strString = root["string"].asString();
		int intNum = root["int"].asInt();

		CONSOLE_INFOS("string %s int %d", strString.c_str(), intNum);
	}
#elif 0 // ����Rabbitmq
	CONSOLE_INFOS("start CRabbitMQ_Publisher...");
	CRabbitMQ_Producer* mq_producer = new CRabbitMQ_Producer();
	mq_producer->SendMsg("", "", "", "Test");
#elif 0
	CONSOLE_INFOS("start CRabbitMQ_Consumer...");
	CRabbitMQ_Consumer* mq_consumer = new CRabbitMQ_Consumer();
	mq_consumer->SetRouteFilter("queueTest","routekeyTest.test",NULL,NULL);
	mq_consumer->StartConsume();
#elif 1
	/* ���ճ�ʱʱ�� */
	timeval timeout;
	timeout.tv_usec = 0;
	timeout.tv_sec = 2;
	/* �������� */
	//std::string strExchange = "MyExchange";
	//std::string strRoutekey = "routekeyTest.test.test";
	//std::string strQueuename = "MyQueue_1";
	CRabbitMQ *m_objRabbitMQ = new CRabbitMQ("localhost", 5672, "root", "root", 1000000);
	m_objRabbitMQ->ConnectRabbitServer();
	/* ���������� */
	//m_objRabbitMQ->ExchangeDeclare("MyExchange_direct", "direct");
	//m_objRabbitMQ->ExchangeDeclare("MyExchange_fanout", "fanout");
	//m_objRabbitMQ->ExchangeDeclare("MyExchange_topic", "topic");
	/* �������� */
	//m_objRabbitMQ->QueueDeclare("MyQueue_direct_1");
	//m_objRabbitMQ->QueueDeclare("MyQueue_fanout_1");
	//m_objRabbitMQ->QueueDeclare("MyQueue_topic_1");
	/* �󶨽���������� */
	//m_objRabbitMQ->QueueBind("MyQueue_direct_1", "MyExchange_direct", "test.direct.*");
	//m_objRabbitMQ->QueueBind("MyQueue_fanout_1", "MyExchange_fanout", "test.fanout.*");
	//m_objRabbitMQ->QueueBind("MyQueue_topic_1", "MyExchange_topic", "test.topic.*");
	m_objRabbitMQ->QueueBind("MyQueue_topic_1", "MyExchange_topic", "test.topic.#");
	/* ȡ������������еİ� */
	//m_objRabbitMQ->QueueUnbind("MyQueue_direct_1", "MyExchange_direct", "test.direct");
	/* ������Ϣ */
	//m_objRabbitMQ->SendMsg("MyExchange_direct", "MyQueue_direct_1", "test.direct.*", "(SendToExchange:MyQueue_direct_1 SendToQueue:MyExchange_direct) with RouteKey:test.direct");
	//m_objRabbitMQ->SendMsg("MyExchange_fanout", "MyQueue_fanout_1", "test.fanout.*", "(SendToExchange:MyQueue_fanout_1 SendToQueue:MyExchange_fanout) with RouteKey:test.fanout.*");
	//m_objRabbitMQ->SendMsg("MyExchange_topic", "MyQueue_topic_1", "test.topic.test", "(SendToExchange:MyExchange_topic SendToQueue:MyExchange_fanout) with RouteKey:test.fanout.test");
	for (auto i = 0; i < 100; i++)
	{
		m_objRabbitMQ->SendMsg("MyExchange_topic", "MyQueue_topic_1", "test.topic.test.test", "(SendToExchange:MyExchange_topic SendToQueue:MyQueue_topic_1) with RouteKey:test.topic.test.test");
	}
	/* ������Ϣ */
	//m_objRabbitMQ->RecvMsg("MyQueue_direct_1", &timeout);
	//m_objRabbitMQ->RecvMsg("MyQueue_fanout_1", &timeout);
	//m_objRabbitMQ->RecvMsg("MyQueue_topic_1", &timeout);
	//m_objRabbitMQ->RecvMsg("MyQueue_topic_1", &timeout);
	/* �̴߳��������Ϣ */
	m_objRabbitMQ->AddQueue("MyQueue_topic_1");
	m_objRabbitMQ->StartConsume();
#elif 0
	
#else
	cout << "Nothing Is Done!" << endl;
#endif
	getchar();

	return 0;
}