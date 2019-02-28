#pragma once
#include "amqp.h"
#include "amqp_framing.h"
#include "amqp_tcp_socket.h"

typedef struct _tagMQ_MESSAGE{
	int nMessageType;
	int nMessageID;
	char szMessageContent[DATA_BUF_SIZE];
}MQ_MESSAGE, *LPMQ_MESSAGE;

/*����/����ʵ����Ϣ����*/
class MQ_Subscriber;
typedef std::multimap<int, MQ_Subscriber*> CSubscriberMultiMap;
typedef std::deque<MQ_MESSAGE> CMessageDeque;
typedef std::vector<MQ_MESSAGE> CMessageVector;
class MQ_Manager;
//������
class MQ_Publisher{
public:
	MQ_Publisher(){};
	~MQ_Publisher(){};

public:
	void PublishMQ(MQ_Manager* pMQManager, int nMessageID, LPMQ_MESSAGE pMessage);
};
//������
class MQ_Subscriber{
public:
	MQ_Subscriber(){};
	~MQ_Subscriber(){};

public:
	void SubscribMQ(MQ_Manager* pMQManager, int nMessageID);
	void UnSubscribMQ(MQ_Manager* pMQManager, int nMessageID);

	void HandleMessage(LPMQ_MESSAGE pMessage);

private:
	CMessageVector			m_vectorMessage;
};
//��Ϣ����
class MQ_Manager{
public:
	MQ_Manager();
	~MQ_Manager(){};

public:
	void AddSubscriber(MQ_Subscriber* pMQSubscriber, int nMessageID);
	void RemoveSubscriber(MQ_Subscriber* pMQSubscriber, int nMessageID);

	void RecvMessage(int nMessageID, LPMQ_MESSAGE pMessage);
	void SendMessage(MQ_Subscriber* pMQSubscriber, LPMQ_MESSAGE pMessage);

	static unsigned __stdcall HandleThreadFunc(LPVOID lpParam);
	void HandleMessage(MQ_MESSAGE message);

public:
	HANDLE					m_hHandleThread;
	CCritSec				m_csSubscriberMapLock;
	CSubscriberMultiMap		m_mapSubscriber;
	CCritSec				m_csMessageDequeLock;
	CMessageDeque			m_dequeMessage;
};

/*����/����ʵ����Ϣ���У�˫������Ϣ����*/
typedef BOOL (*LPCALLBACK_CONSUME_FUNC)(LPMQ_MESSAGE pMessage);
class CMessageQueue{
public:
	CMessageQueue();
	~CMessageQueue();

public:
	void Produce(LPMQ_MESSAGE pMessage);
	void Consume(LPMQ_MESSAGE pMessage);

	static unsigned __stdcall ProduceMessageThread(LPVOID pParam);
	static unsigned __stdcall ConsumeMessageThread(LPVOID pParam);

	CMessageQueue* SetCallbackFunc(LPCALLBACK_CONSUME_FUNC pCallbackFunc);

private:
	CCritSec		m_csProduceLock;
	CMessageDeque	m_dequeProduce;
	unsigned int	m_uiProduceThreadID;
	HANDLE			m_hProduceThread;

	CCritSec		m_csConsumeLock;
	CMessageDeque	m_dequeConsume;
	unsigned int	m_uiConsumeThreadID;
	HANDLE			m_hConsumeThread;
	
	HANDLE			m_hConsumeEvent;
	HANDLE			m_hProduceEvent;

	LPCALLBACK_CONSUME_FUNC m_pCallbackFunc;
};
/*CMessageQueue������*/
class CMessageQueueEx : public CMessageQueue, public CSingle<CMessageQueueEx>{
public:
	CMessageQueueEx(){}
	~CMessageQueueEx(){}
};

/*���rabbitmq-c�ķ�װ*/
typedef int(__stdcall* pRecvMsgCallback)(const std::string& strQueueName, const std::string& strMsg, void* lpParam);
/*RabbitMQ��Ϣ����*/
class CRabbitMsg{
public:
	CRabbitMsg()
	{
		m_pRecvCallback = NULL;
		m_pParam = NULL;
	}

public:
	std::string m_strRouteKey;
	std::string m_strQueueName;
	pRecvMsgCallback m_pRecvCallback;
	void* m_pParam;
};
/*RabbitMQ��װ��*/
class CRabbitMQ{
public:
	CRabbitMQ(std::string strHostName, int nPort, std::string strUserName, std::string strPassword, int nTimeOutMs);
	~CRabbitMQ();

public:
	bool ConnectRabbitServer();
	bool Disconnect();
	
	/**
	* @brief         ExchangeDeclare ����������
	* @param[string] strExchange     ����������
	* @param[string] strType         ����������
	*                "direct"        ֱ�Ӱ󶨡�ʹ�õ�ʱ����Ҫ�����а󶨵��������ϣ���Ϣ��Ҫ��ָ��·�ɼ���ȫƥ�䡣
	*                "fanout"        �㲥��ʹ�õ�ʱ����Ҫ�����а󶨵�����������Ϣ��ת����������ý������󶨵Ķ����С�
	*                "topic"         ƥ�䡣ƥ��ģʽ������·�ɼ�ƥ����Ӧ���������Ķ��У���Ϣ��ת�����������з��������Ķ����С�"#"ƥ��0�������콵��"*"ƥ��һ��������
	* @return        true �����ɹ�   false ����ʧ�ܻ����
	*/
	bool ExchangeDeclare(const string &strExchange, const string &strType);

	/**
	* @brief         QueueDeclare    ������Ϣ����
	* @param[string] strQueueName    ��Ϣ��������
	* @return        true �����ɹ�   false ����ʧ�ܻ����
	*/
	bool QueueDeclare(const string &strQueueName);

	/**
	* @brief         QueueBind       �󶨽����������
	* @param[string] strQueueName    ��������
	* @param[string] strExchange     ����������
	* @param[string] strBindKey      ·�ɹ��򣨲��ܳ���255���ֽڣ�
	* @return        true �󶨳ɹ� false ��ʧ�ܻ����
	*/
	bool QueueBind(const string &strQueueName, const string &strExchange, const string &strBindKey);

	/**
	* @brief         QueueUnbind     ȡ��ָ������������еİ�
	* @param[string] strQueueName    ��������
	* @param[string] strExchange     ����������
	* @param[string] strBindKey      ·�ɹ��򣨲��ܳ���255���ֽڣ�
	* @return        true ����󶨳ɹ� false �����ʧ�ܻ����
	*/
	bool QueueUnbind(const string &strQueueName, const string &strExchange, const string &strBindKey);

	/**
	* @brief         QueueDelete     ɾ��ָ������
	* @param[string] strQueueName    ��������
	* @return        true ɾ�����гɹ� false ɾ������ʧ�ܻ����
	*/
	bool QueueDelete(const string &strQueueName);

	/**
	* @brief         SendMsg         ������Ϣ
	* @param[string] strExchange     ����������
	* @param[string] strQueueName    ��������
	* @param[string] strRouteKey     ·�ɹ��򣨲��ܳ���255���ֽڣ�
	* @param[string] strMessage      ��Ϣ�ַ���
	* @return        true ������Ϣ�ɹ� false ������Ϣʧ�ܻ����
	*                                                          
	* @note          �ڷ�����Ϣ��ʱ��һ��Ҫȷ�����͵�Ŀ�ĵأ�������/���У������Լ�ָ��·����Ч������Ӧ���������ٽ��з�����Ϣ������
	* @note          �����Ҫʵ����Ϣ�־û���ȷ��channel/queue��durable�Լ�����ʱ���Ľṹamqp_basic_properties_t���ֶ�delivery_modeΪ�־û����ԡ�
	*/
	bool SendMsg(const std::string& strExchange, const std::string strQueueName, const std::string& strRouteKey, const std::string& strMessage);
	
	/**
	* @brief         RecvMsg         ������Ϣ
	* @param         strQueueName    ��������
	* @return        true ������Ϣ�ɹ� false ������Ϣʧ�ܻ����
	*                                                          
	* @note          �ڽ�����Ϣ��ʱ��ȷ������Ҫ������Ϣ�Ķ��д��ڣ������޷����յ���Ϣ��
	*/
	bool RecvMsg(const string &strQueueName, struct timeval* timeout);

	/**
	* @brief         HeartBeatFunc   �����̺߳���
	* @return        0 -1
	*/
	static unsigned int __stdcall HeartBeatFunc(LPVOID pParam);
	/**
	* @brief         HeartBeatThread ��������
	* @return        void
	*/
	void HeartBeatThread();

	/**
	* @brief         AddQueue        ��������Ӷ�ָ�����н�����Ϣ
	* @param[string] strQueueName    ��������
	# @return        true �ɹ� false ʧ�ܻ����
	*/
	bool AddQueue(const std::string strQueueName);

	/**
	* @brief         StartConsume    ��ʼ����
	* @return        true �ɹ� false ʧ�ܻ����
	*/
	bool StartConsume();

	/**
	* @brief         ConsumeFunc     �����̺߳���
	* @return        0 -1
	*/
	static unsigned int __stdcall ConsumeFunc(LPVOID pParam);

	/**
	* @brief         ConsumeThread   ���Ѻ���
	* @return        void
	*/
	void ConsumeThread();

	/**
	* @brief         ErrorMsg        ��������
	* @return        ture AMQP_RESPONSE_NORMAL false AMQP Exceptions
	*/
	bool ErrorMsg(amqp_rpc_reply_t x);

private:
	std::string m_strHostName;
	std::string m_strUserName;
	std::string m_strPassword;

	void* m_pConn;
	void* m_pSock;

	int	m_nPort;
	int m_nChannel;

	CCritSec m_csLock;

	struct timeval* m_pConnTimeOut;

	HANDLE m_hHeartBeat;
	unsigned int m_uiHeartBeatThreadId;
	bool m_bHeartBeatFlag;

	HANDLE m_hConsume;
	unsigned int m_uiConsumeThreadId;
	bool m_bConsumeFlag;

	std::vector<std::string> m_vectQueueMap;
};

class CRabbitMQ_Producer{
public:
	CRabbitMQ_Producer();
	~CRabbitMQ_Producer();

private:
	CRabbitMQ* m_objRabbitMQ;
};

class CRabbitMQ_Consumer{
public:
	CRabbitMQ_Consumer();
	~CRabbitMQ_Consumer();

private:
	CRabbitMQ* m_objRabbitMQ;
};