// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <process.h>
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
#include "../Server/common/CommonDef.h"
#include "../Server/common/CommonReq.h"
#include "../Server/include/func.h"
#include "../Server/include/utils.h"
#include "../Server/include/atomic_ops.h"
#include "../Server/include/LockFreeQueue.h"
#include "../Server/iocp.h"