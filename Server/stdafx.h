// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <queue>
#include <map>
#include <deque>
using namespace std;


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "common/CommonDef.h"
#include "common/CommonReq.h"
#include "include/Functions.h"
#include "include/Utilities.h"
#include "include/json/json.h"
#include "threadpool/ThreadPool.h"
#include "threadpool/MessageQueue.h"
#include "iocp/IocpModule.h"
#include "iocp/IocpSocket.h"
#include "iocp/IocpAccept.h"
#include "timer/Timer.h"
#include "db/DBCommon.h"
#include "db/DBOperator.h"
#include "db/mysql/MySQLManager.h"
#include "redis/RedisManager.h"
#include "redis/RedisOperator.h"
#include "CBaseServer.h"
#include "CPlusServer.h"
#include "CPlusClient.h"
#include "HttpManager.h"