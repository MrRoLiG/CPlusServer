// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

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
#include <ctime>
#include <process.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <Mswsock.h>
using namespace std;


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "../Server/common/CommonDef.h"
#include "../Server/common/CommonReq.h"
#include "../Server/include/func.h"
#include "../Server/include/utils.h"
#include "../Server/include/atomic_ops.h"
#include "../Server/include/LockFreeQueue.h"
#include "../Server/iocp.h"
#include "Client.h"