#include "CPacket.h"

CPacket::CPacket()
{
	m_chpBuffer = new char[eBUFFER_DEFAULT];
}

CPacket::CPacket(int iBufferSize)
{
	m_chpBuffer = new char[iBufferSize];
}

CPacket::CPacket(const CPacket& clSrcPacket)
{
	m_iBufferSize = clSrcPacket.m_iBufferSize;
	m_iDataSize = clSrcPacket.m_iDataSize;
	m_iFront = clSrcPacket.m_iFront;
	m_iRear = clSrcPacket.m_iRear;
	
	if (m_chpBuffer != nullptr)
		delete[] m_chpBuffer;

	m_chpBuffer = new char[m_iBufferSize];
	memcpy(m_chpBuffer, clSrcPacket.m_chpBuffer, m_iBufferSize);
}

CPacket::~CPacket()
{
	m_iBufferSize = 0;
	m_iDataSize = 0;
	m_iFront = 0;
	m_iRear = 0;

	if (m_chpBuffer != nullptr)
		delete[] m_chpBuffer;
	
	m_chpBuffer = nullptr;
}

void CPacket::Release(void)
{
	if (m_chpBuffer != nullptr)
		delete[] m_chpBuffer;
}

void CPacket::Clear(void)
{
	m_iFront = 0;
	m_iRear = 0;
}

char* CPacket::GetBufferPtr(void)
{
	return &m_chpBuffer[m_iFront];
}

int CPacket::MoveWritePos(int iSize)
{
	if ( m_iRear + iSize >= m_iBufferSize)
	{
		m_iRear = m_iBufferSize;
		return m_iBufferSize - m_iRear;
	}
	else
	{
		m_iRear += iSize;
		return iSize;
	}
}

int CPacket::MoveReadPos(int iSize)
{
	if (m_iFront + iSize >= m_iRear)
	{
		m_iFront = m_iRear;
		return m_iRear - m_iFront;
	}
	else
	{
		m_iFront += iSize;
		return iSize;
	}
}

int CPacket::GetData(char* chpDest, int iSize)
{
	if (m_iDataSize < iSize)
	{
		// 문제 발생 ( client에서 온 게 문제가 생긴거지 ) -> 예외 던지는 부분
	}

	memcpy(chpDest, m_chpBuffer, iSize);
	MoveReadPos(iSize);
	return iSize;
}

int CPacket::PutData(const char* chpDest, int iSize)
{
	if (m_iDataSize + iSize > m_iBufferSize)
	{
		// 버퍼 크기 키우고 복사하고 로그 찍어야지
	}

	memcpy(m_chpBuffer, chpDest, iSize);
	MoveWritePos(iSize);
	return iSize;
}

CPacket& CPacket::operator=(const CPacket& clSrcPacket)
{
	m_iBufferSize = clSrcPacket.m_iBufferSize;
	m_iDataSize = clSrcPacket.m_iDataSize;
	m_iFront = clSrcPacket.m_iFront;
	m_iRear = clSrcPacket.m_iRear;

	if (m_chpBuffer != nullptr)
		delete[] m_chpBuffer;

	m_chpBuffer = new char[m_iBufferSize];
	memcpy(m_chpBuffer, clSrcPacket.m_chpBuffer, m_iBufferSize);
	
	return *this;
}

CPacket& CPacket::operator<<(unsigned char uchValue)
{
	m_chpBuffer[m_iRear] = uchValue;
	
	// 로그 남기는 위치..
	if (MoveWritePos(sizeof(unsigned char)) < sizeof(unsigned char))
	{
		FILE* fPtr;
		errno_t err = _wfopen_s(&fPtr, L"PACKET_BUFFER_CORRUPTION.txt", L"at,ccs=UTF-16LE");
		WCHAR szMsg[32] = { 0, };
		wsprintfW(szMsg, L"unsigned char, ");
		fwrite(szMsg, wcslen(szMsg) * sizeof(WCHAR), 1, fPtr);
		fclose(fPtr);
		m_bLogCheck = true;

		Resize();
	}

	return *this;
}

CPacket& CPacket::operator<<(char chValue)
{
	m_chpBuffer[m_iRear] = chValue;
	if (MoveWritePos(sizeof(char)) < sizeof(char))
	{
		FILE* fPtr;
		errno_t err = _wfopen_s(&fPtr, L"PACKET_BUFFER_CORRUPTION.txt", L"at,ccs=UTF-16LE");
		WCHAR szMsg[32] = { 0, };
		wsprintfW(szMsg, L"char, ");
		fwrite(szMsg, wcslen(szMsg) * sizeof(WCHAR), 1, fPtr);
		fclose(fPtr);
		m_bLogCheck = true;

		Resize();
	}

	return *this;
}

CPacket& CPacket::operator<<(unsigned short ushValue)
{
	*((unsigned short*)&m_chpBuffer[m_iRear]) = ushValue;
	if (MoveWritePos(sizeof(unsigned short)) < sizeof(unsigned short))
	{
		FILE* fPtr;
		errno_t err = _wfopen_s(&fPtr, L"PACKET_BUFFER_CORRUPTION.txt", L"at,ccs=UTF-16LE");
		WCHAR szMsg[32] = { 0, };
		wsprintfW(szMsg, L"unsigned short, ");
		fwrite(szMsg, wcslen(szMsg) * sizeof(WCHAR), 1, fPtr);
		fclose(fPtr);
		m_bLogCheck = true;

		Resize();
	}

	return *this;
}

CPacket& CPacket::operator<<(short shValue)
{
	*((short*)&m_chpBuffer[m_iRear]) = shValue;
	MoveWritePos(sizeof(short));

	return *this;
}

CPacket& CPacket::operator<<(unsigned int uiValue)
{
	*((unsigned int*)&m_chpBuffer[m_iRear]) = uiValue;
	MoveWritePos(sizeof(unsigned int));

	return *this;
}

CPacket& CPacket::operator<<(int iValue)
{
	*((int*)&m_chpBuffer[m_iRear]) = iValue;
	MoveWritePos(sizeof(int));

	return *this;
}

CPacket& CPacket::operator<<(unsigned long lValue)
{
	*((unsigned long*)&m_chpBuffer[m_iRear]) = lValue;
	MoveWritePos(sizeof(unsigned long));

	return *this;
}

CPacket& CPacket::operator<<(long lValue)
{
	*((long*)&m_chpBuffer[m_iRear]) = lValue;
	MoveWritePos(sizeof(long));

	return *this;
}

CPacket& CPacket::operator<<(__int64 iValue)
{
	*((__int64*)&m_chpBuffer[m_iRear]) = iValue;
	MoveWritePos(sizeof(__int64));

	return *this;
}

CPacket& CPacket::operator<<(float fValue)
{
	*((float*)&m_chpBuffer[m_iRear]) = fValue;
	MoveWritePos(sizeof(float));

	return *this;
}

CPacket& CPacket::operator<<(double dValue)
{
	*((double*)&m_chpBuffer[m_iRear]) = dValue;
	MoveWritePos(sizeof(double));

	return *this;
}




CPacket& CPacket::operator>>(unsigned char& uchValue)
{
	uchValue = *((unsigned char*)&m_chpBuffer[m_iFront]);
	if (MoveReadPos(sizeof(unsigned char)) < sizeof(unsigned char))
		throw Exception(sizeof(unsigned char));

	return *this;
}

CPacket& CPacket::operator>>(char& chValue)
{
	chValue = *((char*)&m_chpBuffer[m_iFront]);
	
	if (MoveReadPos(sizeof(char) < sizeof(char)))
	{
		// size
	}
		

	return *this;
}

CPacket& CPacket::operator>>(unsigned short& ushValue)
{
	ushValue = *((unsigned short*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(unsigned short));

	return *this;
}

CPacket& CPacket::operator>>(short& shValue)
{
	shValue = *((short*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(short));

	return *this;
}

CPacket& CPacket::operator>>(unsigned int& uiValue)
{
	uiValue = *((unsigned int*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(unsigned int));

	return *this;
}

CPacket& CPacket::operator>>(int& iValue)
{
	iValue = *((int*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(int));

	return *this;
}

CPacket& CPacket::operator>>(long& lValue)
{
	lValue = *((long*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(long));

	return *this;
}

CPacket& CPacket::operator>>(__int64& iValue)
{
	iValue = *((__int64*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(__int64));

	return *this;
}

CPacket& CPacket::operator>>(float& fValue)
{
	fValue = *((float*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(float));

	return *this;
}

CPacket& CPacket::operator>>(double& dValue)
{
	dValue = *((double*)&m_chpBuffer[m_iFront]);
	MoveReadPos(sizeof(double));

	return *this;
}

void CPacket::Resize()
{
	char* temp = new char[m_iBufferSize * 2];

	memcpy(temp, m_chpBuffer, m_iBufferSize);
	
	delete m_chpBuffer;
	m_chpBuffer = temp;
}