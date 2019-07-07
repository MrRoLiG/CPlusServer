#include "stdafx.h"

TimeWheel::TimeWheel()
:m_bStop(false), m_uiTick(0)
{
	StartTimer();
}
TimeWheel::~TimeWheel()
{
	m_bStop = true;
}

void TimeWheel::StartTimer()
{
	m_uiTimeWheelNodeID = 0;
	m_lCurrentTime = _getCentiSecond();
	DWORD uiThreadID = 0;
	unsigned int uiID = 0;
	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, _runTimer, this, 0, NULL);
}

void TimeWheel::UpdateTimer()
{
	while (!m_bStop)
	{
		long curTime = _getCentiSecond();
		long timeVal = (curTime - m_lCurrentTime);
		m_lCurrentTime = curTime;
		for (auto i = 0; i < timeVal; i++)
		{
			ExcuteTimerList();		//�ȿ���ǰ��û��ʲô��ʱ������Ҫ����
			MoveTimeWheelList();	//�ƶ���ʱ�����б�,�����Ҫ�ƶ��Ļ�
			ExcuteTimerList();		//�����б��ƶ�֮��Ķ�ʱ����
		}
		Sleep(TIME_INTERVAL);
	}
}

TimeWheelNode* TimeWheel::SetTimer(unsigned int uiMilliSeconds, TimeOutCB TOC, void* pParam, CancelCB CC)
{
	CAutoLock lock(&m_csLock);
	uiMilliSeconds /= TIME_INTERVAL;
	TimeWheelNode* pNode = new TimeWheelNode;
	memset(pNode, 0, sizeof(TimeWheelNode));
	pNode->pParam = pParam;
	pNode->uiExpireTime = uiMilliSeconds + m_uiTick;
	pNode->pTOCB = TOC;
	pNode->pCCB = CC;
	pNode->nTimerID = ++m_uiTimeWheelNodeID;

	m_TimeWheelMap[pNode->nTimerID] = pNode;
	InsertTimeWheelNode(pNode);

	return pNode;
}

void TimeWheel::CancelTimer(unsigned int uiTimerID)
{
	TimeWheelNode* pNode = m_TimeWheelMap[uiTimerID];
	if (NULL == pNode)
	{
		CAutoLock lock(&m_csLock);
		return;
	}
	if (NULL != pNode->pCCB)
	{
		pNode->pCCB(pNode->pParam);
	}
	CAutoLock lock(&m_csLock);
	m_TimeWheelMap.erase(uiTimerID);
	m_TimeWheelList[pNode->uiLevelID][pNode->uiSlotID].pop_back();
	delete pNode;
}

void TimeWheel::ExcuteTimerList()
{
	//�����жϵ�ǰʱ�䴦��������ִ��Ȧ���ĸ��ۿ�
	int nCurrentSlot = (m_uiTick & MASK_INNERMOST);
	//ȡ����Ӧ�ۿڵĶ�ʱ�����б�ȡ�������
	std::list<TimeWheelNode*> nodeList = m_ExcuteList[nCurrentSlot];
	{
		CAutoLock lock(&m_csLock);
		//nodeList = m_ExcuteList[nCurrentSlot];
		m_ExcuteList[nCurrentSlot].clear();
	}
	//ִ�е�ǰʱ������������б��д�����Ķ�ʱ����
	for (auto iter = nodeList.begin(); iter != nodeList.end(); iter++)
	{
		TimeWheelNode* pNode = *iter;
		pNode->pTOCB(pNode->pParam);
		{
			CAutoLock lock(&m_csLock);
			//ɾ���洢��map�еĶ�ʱ��������
			m_TimeWheelMap.erase(pNode->nTimerID);
		}
		delete pNode;
	}
}

void TimeWheel::MoveTimeWheelList()
{
	//m_nTick����
	unsigned int uiCurrentTick = ++m_uiTick;
	//����uiMask,uiTime,i��ʱ����
	unsigned int uiMask = (1 << BIT_INNER);
	unsigned int uiTime = (uiCurrentTick >> BIT_INNER);
	int i = 0;
	//��ǰʱ�����ϣ�1111 1111��������0����˵����ǰ��ʱΪ256�ı�����Ҳ����ʱ����ת��һȦ
	//��ʱ����Ӧ��ȦӦ��ת��һ��ͬʱ����ǰת���ĸ����е����ж�ʱ�������½��з���
	while ((uiCurrentTick & (uiMask - 1)) == 0)
	{
		unsigned int uiMoveSlot = (uiTime & MASK_OUTLEVELS);
		//����0������ǰʱ�䲻���ڵ�ǰ�㼶������Ȧ������ѭ�����б���
		if (uiMoveSlot != 0)
		{
			//��������
			MoveTimeWheelListEx(m_TimeWheelList[i][uiMoveSlot]);
			m_TimeWheelList[i][uiMoveSlot].clear();
			break;
		}
		uiTime >>= BIT_LEVEL;
		uiMask <<= BIT_LEVEL;
		//�㼶������
		++i;
	}
}

void TimeWheel::MoveTimeWheelListEx(std::list<TimeWheelNode*>& nodeList)
{
	CAutoLock lock(&m_csLock);
	//�����ò��ϵ����ж�ʱ�������²��뵽ʱ������
	for (auto iter = nodeList.begin(); iter != nodeList.end();)
	{
		TimeWheelNode* pNode = *iter;
		InsertTimeWheelNode(pNode);
		iter = nodeList.erase(iter++);
	}
}

void TimeWheel::InsertTimeWheelNode(TimeWheelNode* pNode)
{
	//��ǰnodeʣ�ൽ��ʱ�䣬��λms
	unsigned int uiRemainTime = pNode->uiExpireTime - m_uiTick;
	//��ǰnodeʣ�ൽ��ʱ����256���ڣ����Ѿ�����Ҫִ�е�ʱ���ˣ������������ִ��Ȧ��Ӧ�ۿ�
	if ((uiRemainTime | MASK_INNERMOST) == MASK_INNERMOST)
	{
		//����node��Ҫ�����λ��
		unsigned int uiInsertSlot = (uiRemainTime & MASK_INNERMOST);
		pNode->bToExcute = true;
		pNode->uiSlotID = uiInsertSlot;
		m_ExcuteList[uiInsertSlot].push_back(pNode);
	}
	//���Ǽ���ִ�еģ����·�����λ��
	else
	{
		unsigned int uiMask = ((1 << BIT_INNER) << BIT_LEVEL);
		int i = 0;
		for (i = 0; i < TIME_WHEEL_LEVEL_SLOT; i++)
		{
			//��ǰnodeʣ�����ʱ��������ڸò㼶ʱ��ȡ�øò㼶��Ӧ���룬�������ѭ��
			if ((uiRemainTime | (uiMask - 1)) == (uiMask - 1))
			{
				break;
			}
			uiMask <<= BIT_LEVEL;
		}
		//���㵱ǰnode�ڵ�ǰ�㼶�����ۿ�
		unsigned int uiInsertSlot = ((pNode->uiExpireTime >> (BIT_INNER + i * BIT_LEVEL)) & (uiMask - 1));
		pNode->bToExcute = false;
		pNode->uiLevelID = i;
		pNode->uiSlotID = uiInsertSlot;
		m_TimeWheelList[i][uiInsertSlot].push_back(pNode);
	}
}

unsigned TimeWheel::_runTimer(void* pParam)
{
	TimeWheel* pTimeWheel = (TimeWheel*)pParam;
	pTimeWheel->UpdateTimer();
	return 0;
}