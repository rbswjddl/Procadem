
#ifndef __PACKET_H__
#define __PACKET_H__

#include<iostream>
#include<Windows.h>

class CPacket
{
public:
	class Exception
	{
	public:
		Exception(int iSize)
		{
			size = iSize;
		}
	
	private:
		int size;
	};

public:
	enum en_PACKET
	{
		eBUFFER_DEFAULT			= 1400
	};

	CPacket();
	CPacket(int iBufferSize);
	CPacket(const CPacket& clSrcPacket);
	virtual ~CPacket();

	void	Release(void);
	void	Clear(void);

	char*	GetBufferPtr(void);
	
	int		GetBufferSize(void) { return m_iBufferSize; }
	int		GetDataSize(void) { return m_iDataSize; }

	int		MoveWritePos(int iSize);
	int		MoveReadPos(int	iSize);

	int		GetData(char* chpDest, int iSize);
	int		PutData(const char* chpDest, int iSize);

private:
	void	Resize();

public:
	CPacket& operator= (const CPacket& clSrcPacket);

	// 로그 찍기
	CPacket& operator<< (unsigned char		uchValue);
	CPacket& operator<< (char				chValue);
	CPacket& operator<< (unsigned short		ushValue);
	CPacket& operator<< (short				shValue);
	CPacket& operator<< (unsigned int		uiValue);
	CPacket& operator<< (int				iValue);
	CPacket& operator<< (unsigned long		lValue);
	CPacket& operator<< (long				lValue);
	CPacket& operator<< (__int64			iValue);
	CPacket& operator<< (float				fValue);
	CPacket& operator<< (double				dValue);

	// throw 던진다는 거 표시해야된다.
	CPacket& operator>> (unsigned char&		uchValue)	throw (Exception);
	CPacket& operator>> (char&				chValue)	throw (Exception);
	CPacket& operator>> (unsigned short&	ushValue)	throw (Exception);
	CPacket& operator>> (short&				shValue)	throw (Exception);
	CPacket& operator>> (unsigned int&		uiValue)	throw (Exception);
	CPacket& operator>> (int&				iValue)		throw (Exception);
	CPacket& operator>> (long&				lValue)		throw (Exception);
	CPacket& operator>> (__int64&			iValue)		throw (Exception);
	CPacket& operator>> (float&				fValue)		throw (Exception);
	CPacket& operator>> (double&			dValue)		throw (Exception);

protected:
	char*			m_chpBuffer = nullptr;
	int				m_iBufferSize = eBUFFER_DEFAULT;
	int				m_iDataSize = 0;
	int				m_iFront = 0;
	int				m_iRear = 0;

	bool			m_bLogCheck = false;
};

#endif