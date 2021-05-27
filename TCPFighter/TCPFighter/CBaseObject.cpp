#include<Windows.h>
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"
#include "PacketStruct.h"
#include "LinkedList.h"
// a
extern CList<CBaseObject*> g_List;

CBaseObject::CBaseObject(DWORD objectID, int objectType, USHORT ushX, USHORT ushY)
	:m_dwObjectID(objectID), m_iObjectType(objectType), m_ushCurX(ushX), m_ushCurY(ushY)
{
	m_bEndFrame = FALSE;
	m_dwActionInput = 0;

	m_iDelayCount = 0;
	m_iFrameDelay = 0;

	m_iSpriteStart = 0;
	m_iSpriteNow = 0;
	m_iSpriteEnd = 0;
	m_iSpriteMax = 0;
}

CBaseObject::~CBaseObject()
{

}

void CBaseObject::ActionInput(DWORD dwAction)
{
	m_dwActionInput = dwAction;
}

USHORT CBaseObject::GetCurX()
{
	return m_ushCurX;
}

USHORT CBaseObject::GetCurY()
{
	return m_ushCurY;
}

DWORD CBaseObject::GetObjectID()
{
	return m_dwObjectID;
}

int CBaseObject::GetObjectType()
{
	return m_iObjectType;
}

int CBaseObject::GetSprite()
{
	return m_iSpriteNow;
}

BOOL CBaseObject::IsEndFrame()
{
	return m_bEndFrame;
}

void CBaseObject::NextFrame()
{
	if (m_iSpriteStart < 0)
		return;

	m_iDelayCount++;

	if (m_iDelayCount >= m_iFrameDelay)
	{
		m_iDelayCount = 0;

		m_iSpriteNow++;

		if (m_iSpriteNow > m_iSpriteMax)
		{
			m_iSpriteNow = m_iSpriteStart;
			m_bEndFrame = TRUE;
		}
	}
}

BOOL CBaseObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	return TRUE;
}

void CBaseObject::Run()
{}

void CBaseObject::SetPosition(USHORT ushX, USHORT ushY)
{
	m_ushCurX = ushX;
	m_ushCurY = ushY;
}

void CBaseObject::SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay)
{
	m_iSpriteStart = iSpriteStart;
	m_iSpriteMax = iSpriteMax;
	m_iFrameDelay = iFrameDelay;

	m_iSpriteNow = iSpriteStart;
	m_iDelayCount = 0;
	m_bEndFrame = FALSE;
}