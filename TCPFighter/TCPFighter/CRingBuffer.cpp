#include "CRingBuffer.h"
#include<cstring>

CRingBuffer::CRingBuffer(void)
{
	m_iBufferSize = BUFFER_SIZE;
	m_iFront = 0;
	m_iRear = 0;

	m_queue = new char[BUFFER_SIZE];
	memset(m_queue, 0, BUFFER_SIZE);
}

CRingBuffer::CRingBuffer(int iBufferSize)
{
	m_iBufferSize = iBufferSize;
	m_iFront = 0;
	m_iRear = 0;

	m_queue = new char[iBufferSize];
	memset(m_queue, 0, iBufferSize);
}

CRingBuffer::~CRingBuffer(void)
{
	if (m_queue != nullptr)
	{
		delete[] m_queue;
	}

	m_iFront = 0;
	m_iRear = 0;
	m_iBufferSize = 0;
}

void CRingBuffer::Resize(int iSize)
{
	//re-try
	if (m_queue == nullptr)
		return;

	char* temp = new char[iSize];

	if (m_iRear >= m_iFront)
	{
		memcpy(temp, m_queue, iSize); // 크기 수정
	}
	else
	{
		m_iRear = m_iBufferSize + m_iRear - 1;
		memcpy(&temp[m_iFront + 1], &m_queue[m_iFront + 1], m_iBufferSize - m_iFront - 1);
		memcpy(&temp[m_iBufferSize], &m_queue[0], m_iRear + 1);
	}
	m_iBufferSize = iSize;

	delete[] m_queue;
	m_queue = temp;
}

int CRingBuffer::GetBufferSize(void)
{
	return m_iBufferSize - 1;
}

int CRingBuffer::GetUseSize(void)
{
	int iUseSize;

	if (NextIndex(m_iRear) == m_iFront)
		return m_iBufferSize - 1;

	if (m_iRear >= m_iFront)
	{
		// 수정한 부분
		iUseSize = m_iRear - m_iFront;
	}
	else
	{
		iUseSize = m_iBufferSize - m_iFront + m_iRear;
	}

	return iUseSize;
}

int CRingBuffer::GetFreeSize(void)
{
	return m_iBufferSize - GetUseSize() - 1;
}

int CRingBuffer::DirectEnqueueSize(void)
{
	int enqueSize;
	if (m_iFront > m_iRear)
	{
		enqueSize = m_iFront - m_iRear - 1;
	}
	else
	{
		enqueSize = m_iBufferSize - m_iRear - 1;
	}

	return enqueSize;
}

int CRingBuffer::DirectDequeueSize(void)
{
	int dequeSize;

	if (m_iRear >= m_iFront)
	{
		dequeSize = m_iRear - m_iFront;
	}
	else
	{
		dequeSize = (m_iBufferSize - 1) - m_iFront;
	}

	return dequeSize;
}

bool CRingBuffer::Enqueue(char* chpData, int iSize)
{
	if (GetFreeSize() < iSize)
		return false;

	int cpySize;
	if (m_iRear + iSize > m_iBufferSize - 1)
	{
		cpySize = (m_iBufferSize - 1) - m_iRear;
		memcpy(&m_queue[m_iRear + 1], chpData, cpySize);
		memcpy(&m_queue[0], &chpData[cpySize], iSize - cpySize);
		m_iRear = iSize - cpySize - 1;
	}
	else
	{
		memcpy(&m_queue[m_iRear + 1], chpData, iSize);
		m_iRear += iSize;
	}

	return true;
}

bool CRingBuffer::Dequeue(char* chpData, int iSize)
{
	if (GetUseSize() < iSize)
		return false;

	int cpySize;
	if (m_iBufferSize - m_iFront - 1 < iSize)
	{
		cpySize = (m_iBufferSize - 1) - m_iFront;
		memcpy(chpData, &m_queue[m_iFront + 1], cpySize);
		memcpy(&chpData[cpySize], &m_queue[0], iSize - cpySize);
		m_iFront = iSize - cpySize - 1;
	}
	else
	{
		memcpy(chpData, &m_queue[m_iFront + 1], iSize);
		m_iFront += iSize;
	}

	return true;
}

bool CRingBuffer::Peek(char* chpData, int iSize)
{
	if (GetUseSize() < iSize)
		return false;

	int cpySize;
	if (m_iBufferSize - m_iFront - 1 < iSize)
	{
		cpySize = (m_iBufferSize - 1) - m_iFront;
		memcpy(chpData, &m_queue[m_iFront + 1], cpySize);
		memcpy(&chpData[cpySize], &m_queue[0], iSize - cpySize);
	}
	else
	{
		memcpy(chpData, &m_queue[m_iFront + 1], iSize);
	}

	return true;
}

void CRingBuffer::MoveRear(int iSize)
{
	if (iSize > m_iBufferSize - 1)
		return;

	if (m_iRear + iSize > (m_iBufferSize - 1))
	{
		m_iRear = iSize - (m_iBufferSize - m_iRear);
	}
	else
	{
		m_iRear += iSize;
	}
}

void CRingBuffer::MoveFront(int iSize)
{
	if (iSize > m_iBufferSize - 1)
		return;

	if (m_iFront + iSize > (m_iBufferSize - 1))
	{
		m_iFront = iSize - (m_iBufferSize - m_iFront);
	}
	else
	{
		m_iFront += iSize;
	}
}

void CRingBuffer::ClearBuffer(void)
{
	m_iFront = 0;
	m_iRear = 0;
}

char* CRingBuffer::GetFrontBufferPtr(void)
{
	// 수정한 부분
	//return &m_queue[m_iFront];
	return &m_queue[m_iFront + 1];
}

char* CRingBuffer::GetRearBufferPtr(void)
{
	//수정한 부분
	//return &m_queue[m_iRear]
	return &m_queue[m_iRear + 1];
}

int CRingBuffer::NextIndex(int idx)
{
	if (idx + 1 >= m_iBufferSize)
		return 0;
	else
		return idx + 1;
}

int CRingBuffer::GetRearPos()
{
	return m_iRear;
}

int CRingBuffer::GetFrontPos()
{
	return m_iFront;
}