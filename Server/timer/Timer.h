#pragma once

#include<list>
#include<map>
#include<windows.h>
#include<process.h>
#include<time.h>

#define TIME_WHEEL_LEVEL_SLOT		4						//ʱ���ֵ��ĸ��㼶
#define TIME_INTERVAL				20						//����Ϊ20ms
#define BIT_INNER					8
#define BIT_LEVEL					6

#define SLOT_INNERMOST				(1 << BIT_INNER)		//������ִ��Ȧ��������� ��0001 0000 0000��
#define SLOT_OUTLEVELS				(1 << BIT_LEVEL)		//������ִ��Ȧ����ĸ��㼶ÿ���㼶�����Ȧ���Ĳ�����0100 0000��
#define MASK_INNERMOST				((1 << BIT_INNER) - 1)	//�ж������Ƿ��ƶ���������ִ��Ȧ�����루1111 1111��
#define MASK_OUTLEVELS				((1 << BIT_LEVEL) - 1)	//�ж�������������Ȧ�����루0011 1111��

//�ص�����ָ��
typedef void(*TimeOutCB)(void*);
typedef void(*CancelCB)(void*);

//ʱ���ֲ���
struct TimeWheelNode{
	unsigned int nTimerID;
	void* pParam;
	TimeOutCB pTOCB;
	CancelCB pCCB;
	unsigned int uiExpireTime;
	unsigned int uiLevelID;
	unsigned int uiSlotID;
	bool bToExcute;
};

class TimeWheel{

public:
	TimeWheel();
	~TimeWheel();

	void StartTimer();
	void UpdateTimer();

	TimeWheelNode* SetTimer(unsigned int uiMilliSeconds, TimeOutCB TOC, void* pParam, CancelCB CC);
	void CancelTimer(unsigned int uiTimerID);

	void ExcuteTimerList();
	void MoveTimeWheelList();
	void MoveTimeWheelListEx(std::list<TimeWheelNode*>& nodeList);

	void InsertTimeWheelNode(TimeWheelNode* pNode);

private:
	static unsigned int __stdcall _runTimer(void* pParam);

private:
	bool m_bStop;
	unsigned int m_uiTimeWheelNodeID;
	unsigned int m_uiTick;
	long m_lCurrentTime;
	CCritSec m_csLock;

	std::list<TimeWheelNode*> m_TimeWheelList[TIME_WHEEL_LEVEL_SLOT][SLOT_OUTLEVELS];
	std::list<TimeWheelNode*> m_ExcuteList[SLOT_INNERMOST];
	std::map<int, TimeWheelNode*> m_TimeWheelMap;
};