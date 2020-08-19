#pragma once

#include <mutex>
#include <windows.h>
#include "def.h"
#include "log.h"
#include "lock.h"

class CBuffer
{
public:
	CBuffer()
	{
		m_uiSize = 0;
		m_pBase = nullptr;
		m_pPtr = m_pBase;
	}
	virtual ~CBuffer()
	{
		ClearBuffer();
	}
	/*����������*/
	void ClearBuffer()
	{
		SAFE_DELETE_ARRAY(m_pBase);
		m_pPtr = nullptr;
		m_uiSize = 0;
	}
	/*��ȡ��ǰ���ݳ���*/
	UINT GetBufferLen()
	{
		return (UINT)(m_pPtr - m_pBase);
	}
	/*�������ض�ȡ�����ݳ��ȣ�bUpdate��־��ʾ����ʱ��ȡ�����ݸ��»�����*/
	UINT Read(PBYTE pData, UINT uiSize, BOOL bUpdate = TRUE)
	{
		UINT uiDataLen = GetBufferLen();
		if (uiDataLen <= 0 || !pData)
		{
			return 0;
		}
		else
		{
			if (uiSize > uiDataLen)
			{
				/*��ȡ���ȳ������ݱ����ȣ���������*/
				return 0;
			}
			else
			{
				/*���ض�ȡ�����ݳ���*/
				memcpy(pData, m_pBase, uiSize);
				if (bUpdate)
				{
					/*�����Ҫ����ȡ������ǰ��*/
					memmove(m_pBase, m_pBase + uiSize, m_uiSize - uiSize);
					m_pPtr -= uiSize;
				}
				return uiSize;
			}
		}
	}
	/*д������д������ݳ���*/
	UINT Write(PBYTE pData, UINT uiSize)
	{
		if (uiSize <= 0 || !pData)
		{
			return 0;
		}
		else
		{
			UINT uiRemainLen = m_uiSize - GetBufferLen();
			if (uiRemainLen >= uiSize)
			{
				/*�ռ��㹻��ֱ�ӿ����ڴ�*/
				memcpy(m_pPtr, pData, uiSize);
				m_pPtr += uiSize;
			}
			else
			{
				/*�ռ䲻�㣬���������ڴ�*/
				UINT uiOriginLen = ReAllocateBuffer(uiSize);
				memcpy(m_pPtr, pData, uiSize);
				m_pPtr += uiSize;
			}
			return uiSize;
		}
	}
	/*��ȡ����ͷָ��*/
	PBYTE GetBuffer()
	{
		return m_pBase;
	}

protected:
	/*���������ڴ�ռ�*/
	UINT ReAllocateBuffer(UINT uiRequestedSize)
	{
		UINT uiOriginLen = GetBufferLen();
		if (uiOriginLen >= 0)
		{
			PBYTE pTemp = new BYTE[uiOriginLen];
			memcpy(pTemp, m_pBase, uiOriginLen);
			SAFE_DELETE_ARRAY(m_pBase);

			UINT uiAllocateLen = m_uiSize + (int)(uiRequestedSize / 1024 + 1) * 1024;
			m_pBase = new BYTE[uiAllocateLen];
			memcpy(m_pBase, pTemp, uiOriginLen);
			SAFE_DELETE_ARRAY(pTemp);

			m_pPtr = m_pBase + uiOriginLen;
			m_uiSize = uiAllocateLen;
			return uiOriginLen;
		}
		return 0;
	}
	/*�ͷŶ����ڴ�ռ�*/
	UINT DeAllocateBuffer(UINT uiRequestedSize)
	{
		UINT uiOriginLen = GetBufferLen();
		if (uiOriginLen > 0)
		{
			/*�ͷŵĳ��ȴ��ڵ�ǰ�������ݳ��ȣ�����������*/
			if (uiRequestedSize >= uiOriginLen)
			{
				ClearBuffer();
				return uiOriginLen;
			}
			else
			{
				PBYTE pTemp = new BYTE[uiOriginLen];
				memcpy(pTemp, m_pBase, uiOriginLen);
				SAFE_DELETE_ARRAY(m_pBase);

				UINT uiAllocateLen = (int)((uiOriginLen - uiRequestedSize) / 1024 + 1) * 1024;
				m_pBase = new BYTE[uiAllocateLen];
				memcpy(m_pBase, pTemp + uiRequestedSize, uiOriginLen);
				SAFE_DELETE_ARRAY(pTemp);

				m_pPtr = m_pBase + uiOriginLen;
				m_uiSize = uiAllocateLen;
				return (uiAllocateLen - m_uiSize);
			}
		}
		return 0;
	}
	/*��ȡ��ǰ������ڴ�ռ�*/
	UINT GetMemSize()
	{
		return m_uiSize;
	}

protected:
	PBYTE	m_pBase;
	PBYTE	m_pPtr;
	UINT	m_uiSize;
};

class IDataBuffer
{
public:
	virtual UINT GetBufferLen() = 0;
	virtual BYTE* GetBuffer() = 0;
	virtual UINT GetDataLen() = 0;
	virtual BYTE* GetData() = 0;
	virtual void SetDataLen(UINT nLen) = 0;
	virtual void SetBufferLen(UINT nLen) = 0;
	virtual void Release() = 0;
};

template<UINT SIZE>
class CDataBufferList;

template<UINT SIZE>
class CDataBuffer : public IDataBuffer
{
public:
	CDataBuffer()
	{
		m_pPrev = nullptr;
		m_pNext = nullptr;
		m_pManager = nullptr;

		ZeroMemory(&m_Buffer, SIZE);
		m_nBufSize = SIZE;
		m_nDataLen = 0;
	}
	virtual ~CDataBuffer()
	{

	}

public:
	UINT GetBufferLen()
	{
		return m_nDataLen;
	}
	BYTE* GetBuffer()
	{
		return (BYTE*)m_Buffer;
	}
	UINT GetDataLen()
	{
		return m_nDataLen;
	}
	BYTE* GetData()
	{
		return (BYTE*)m_Buffer + 0;
	}
	void SetDataLen(UINT nLen)
	{
		m_nDataLen = nLen;
	}
	void SetBufferLen(UINT nLen)
	{
		m_nBufSize = nLen;
	}
	void Release()
	{
		m_pManager->ReleaseDataBuffer(this);
	}

private:
	CDataBuffer(const CDataBuffer&) = delete;
	CDataBuffer& operator=(const CDataBuffer&) = delete;

public:
	CDataBuffer<SIZE>* m_pPrev;
	CDataBuffer<SIZE>* m_pNext;
	CDataBufferList<SIZE>* m_pManager;
	UINT m_nDataLen;
	UINT m_nBufSize;
	BYTE m_Buffer[SIZE];
};

template<UINT SIZE>
class CDataBufferList
{
public:
	CDataBufferList()
	{
		m_llFreeCount = 0;
		m_llBusyCount = 0;
		m_pFreeBufferList = nullptr;
		m_pBusyBufferList = nullptr;
	}
	virtual ~CDataBufferList()
	{

	}

public:
	IDataBuffer* AllocateDataBuffer(UINT nDataLen)
	{
		std::lock_guard<std::mutex> lock(m_lock);
		CDataBuffer<SIZE>* pBuffer = nullptr;
		if (!m_pFreeBufferList)
		{
			pBuffer = new CDataBuffer<SIZE>();
			pBuffer->m_pManager = this;
		}
		else
		{
			pBuffer = m_pFreeBufferList;
			m_pFreeBufferList = m_pFreeBufferList->m_pNext;
			if (m_pFreeBufferList)
			{
				m_pFreeBufferList->m_pPrev = nullptr;
			}
			m_llFreeCount--;
		}
		ZeroMemory(pBuffer->m_Buffer, SIZE);
		pBuffer->SetBufferLen(SIZE);
		pBuffer->SetDataLen(nDataLen);
		pBuffer->m_pNext = nullptr;
		pBuffer->m_pPrev = nullptr;
		if (!m_pBusyBufferList)
		{
			m_pBusyBufferList = pBuffer;
			m_pBusyBufferList->m_pPrev = nullptr;
		}
		else
		{
			pBuffer->m_pNext = m_pBusyBufferList;
			m_pBusyBufferList->m_pPrev = pBuffer;
			pBuffer->m_pPrev = nullptr;
			m_pBusyBufferList = pBuffer;
		}
		m_llBusyCount++;
		//myLogConsoleI("AllocateDataBuffer[%d] nDataLen:%d GetFreeCount:%lld GetBusyCount %lld", SIZE, nDataLen, m_llFreeCount, m_llBusyCount);
		return pBuffer;
	}
	bool ReleaseDataBuffer(CDataBuffer<SIZE>* pBuffer)
	{
		if (!pBuffer)
		{
			return false;
		}
		else
		{
			std::lock_guard<std::mutex> lock(m_lock);
			if (pBuffer == m_pBusyBufferList)
			{
				m_pBusyBufferList = pBuffer->m_pNext;
				if (m_pBusyBufferList)
				{
					m_pBusyBufferList->m_pPrev = nullptr;
				}
			}
			else
			{
				if (!pBuffer->m_pPrev)
				{
					return false;
				}
				pBuffer->m_pPrev->m_pNext = pBuffer->m_pNext;
				if (pBuffer->m_pNext)
				{
					pBuffer->m_pNext->m_pPrev = pBuffer->m_pPrev;
				}
			}

			pBuffer->m_pNext = nullptr;
			pBuffer->m_pPrev = nullptr;
			pBuffer->m_nDataLen = 0;
			ZeroMemory(pBuffer->m_Buffer, SIZE);

			m_llBusyCount--;
			if (!m_pFreeBufferList)
			{
				m_pFreeBufferList = pBuffer;
			}
			else
			{
				pBuffer->m_pNext = m_pFreeBufferList;
				m_pFreeBufferList->m_pPrev = pBuffer;
				m_pFreeBufferList = pBuffer;
			}
			m_llFreeCount++;
			//myLogConsoleI("ReleaseDataBuffer[%d] GetFreeCount:%lld GetBusyCount %lld", SIZE, m_llFreeCount, m_llBusyCount);
			return true;
		}
	}
	void Release()
	{
		std::lock_guard<std::mutex> lock(m_lock);
		CDataBuffer<SIZE>* pTemp = nullptr;
		CDataBuffer<SIZE>* pBuffer = m_pFreeBufferList;
		while (pBuffer)
		{
			pTemp = pBuffer;
			pBuffer = pBuffer->m_pNext;
			pTemp->m_nDataLen = 0;
			ZeroMemory(pTemp->m_Buffer, SIZE);
			delete pTemp;
		}
		m_llFreeCount = 0;
		pBuffer = m_pBusyBufferList;
		while (pBuffer)
		{
			pTemp = pBuffer;
			pBuffer = pBuffer->m_pNext;
			pTemp->m_nDataLen = 0;
			ZeroMemory(pTemp->m_Buffer, SIZE);
			delete pTemp;
		}
		m_llBusyCount = 0;
	}

private:
	CDataBufferList(const CDataBufferList&) = delete;
	CDataBufferList& operator=(const CDataBufferList&) = delete;

private:
	CDataBuffer<SIZE>* m_pFreeBufferList;
	CDataBuffer<SIZE>* m_pBusyBufferList;
	LONGLONG m_llFreeCount;
	LONGLONG m_llBusyCount;
	std::mutex m_lock;
};

#define BUFFER_SIZE_032B 32
#define BUFFER_SIZE_064B 64
#define BUFFER_SIZE_128B 128
#define BUFFER_SIZE_256B 256
#define BUFFER_SIZE_512B 512
#define BUFFER_SIZE_01KB 1024
#define BUFFER_SIZE_02KB 2048
#define BUFFER_SIZE_04KB 4096
#define BUFFER_SIZE_08KB 8192
#define BUFFER_SIZE_10MB 10*1024*1024
class CDataBufferMgr
{
public:
	CDataBufferMgr()
	{

	}
	virtual ~CDataBufferMgr()
	{

	}

public:
	IDataBuffer* AllocateDataBuffer(const UINT& nSize)
	{
		IDataBuffer* pBuffer = nullptr;
		if (nSize <= BUFFER_SIZE_032B)
		{
			pBuffer = m_032B.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_064B)
		{
			pBuffer = m_064B.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_128B)
		{
			pBuffer = m_128B.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_256B)
		{
			pBuffer = m_256B.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_512B)
		{
			pBuffer = m_512B.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_01KB)
		{
			pBuffer = m_01KB.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_02KB)
		{
			pBuffer = m_02KB.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_04KB)
		{
			pBuffer = m_04KB.AllocateDataBuffer(nSize);
		}
		else if (nSize <= BUFFER_SIZE_08KB)
		{
			pBuffer = m_08KB.AllocateDataBuffer(nSize);
		}
		else
		{
			pBuffer = m_10MB.AllocateDataBuffer(nSize);
		}
		return pBuffer;
	}
	UINT GetKey(const UINT& nDataLen)
	{
		UINT nMin = 1 << 4;
		UINT nMax = 1 << 16;
		if (nDataLen <= 0 || nDataLen > nMax)
		{
			return 0;
		}
		UINT nTmp = nDataLen - 1;
		nTmp |= nTmp >> 1;
		nTmp |= nTmp >> 2;
		nTmp |= nTmp >> 4;
		nTmp |= nTmp >> 8;
		nTmp |= nTmp >> 16;
		nTmp = nTmp + 1;
		if (nTmp <= nMin)
		{
			return nMin;
		}
		else if (nTmp >= nMax)
		{
			return nMax;
		}
		else
		{
			return nTmp;
		}
	}

private:
	CDataBufferMgr(const CDataBufferMgr&) = delete;
	CDataBufferMgr& operator=(const CDataBufferMgr&) = delete;

private:
	CDataBufferList<BUFFER_SIZE_032B> m_032B;
	CDataBufferList<BUFFER_SIZE_064B> m_064B;
	CDataBufferList<BUFFER_SIZE_128B> m_128B;
	CDataBufferList<BUFFER_SIZE_256B> m_256B;
	CDataBufferList<BUFFER_SIZE_512B> m_512B;
	CDataBufferList<BUFFER_SIZE_01KB> m_01KB;
	CDataBufferList<BUFFER_SIZE_02KB> m_02KB;
	CDataBufferList<BUFFER_SIZE_04KB> m_04KB;
	CDataBufferList<BUFFER_SIZE_08KB> m_08KB;
	CDataBufferList<BUFFER_SIZE_10MB> m_10MB;
};
