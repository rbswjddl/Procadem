#include<Windows.h>
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"
#include "CSpriteDib.h"
#include "CScreenDib.h"
#include "LinkedList.h"

extern CSpriteDib g_SpriteDib;
extern CList<CBaseObject*> g_List;
extern CBaseObject* g_pPlayerObject;



CEffectObject::CEffectObject(DWORD objectID, int objectType, USHORT ushX, USHORT ushY, DWORD attackID)
	:CBaseObject(objectID, objectType, ushX, ushY)
{
	m_bEffectStart = FALSE;
	m_dwAttackID = attackID;
	m_pAttacker = nullptr;

	auto iter = g_List.begin();
	for (; iter != g_List.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == m_dwAttackID)
		{
			m_byCurDir = ((CPlayerObject*)(*iter))->GetDirection();
			break;
		}
	}

	// 만약에 도중에 얘가 가리키는 플레이어가 delete된다면??.... 이런 방법 위험하다.
	// 예외처리를 생각 못해줬네...
	/*auto iter = g_List.begin();
	for (; iter != g_List.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == m_dwAttackID)
		{
			m_pAttacker = (CPlayerObject*)(*iter);
		}
	}*/
}

CEffectObject::~CEffectObject()
{

}

BOOL CEffectObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	if (m_bEffectStart == TRUE)
	{
		if (m_byCurDir == LL)
			g_SpriteDib.DrawSprite(GetSprite(), GetCurX() - 50, GetCurY() - 65, bypDest, iDestWidth, iDestHeight, iDestPitch);
		else
			g_SpriteDib.DrawSprite(GetSprite(), GetCurX() + 50, GetCurY() - 65, bypDest, iDestWidth, iDestHeight, iDestPitch);
	}
	return TRUE;
}

void CEffectObject::Run()
{
	int iSpriteNow;
	BOOL isAttacker = FALSE;

	auto iter = g_List.begin();
	for (; iter != g_List.end(); ++iter)
	{
		if ((*iter)->GetObjectID() == m_dwAttackID)
		{
			m_pAttacker = (CPlayerObject*)(*iter);
			iSpriteNow = (*iter)->GetSprite();
			isAttacker = TRUE;
			break;
		}
	}

	if (isAttacker == FALSE)
	{
		while (1)
		{
			if (IsEndFrame())
				break;
			NextFrame();
		}
	}
	else
	{
		if (m_pAttacker->GetDirection() == LL)
		{
			if (iSpriteNow >= ePLAYER_ATTACK3_L01 && iSpriteNow <= ePLAYER_ATTACK3_L06)
			{
				if (iSpriteNow >= ePLAYER_ATTACK3_L05)
					m_bEffectStart = TRUE;

			}
			else if (iSpriteNow >= ePLAYER_ATTACK2_L01 && iSpriteNow <= ePLAYER_ATTACK2_L04)
			{
				if (iSpriteNow >= ePLAYER_ATTACK2_L03)
					m_bEffectStart = TRUE;
			}
			else
			{
				if (iSpriteNow >= ePLAYER_ATTACK1_L03)
					m_bEffectStart = TRUE;
			}
		}
		else
		{
			if (iSpriteNow >= ePLAYER_ATTACK3_R01 && iSpriteNow <= ePLAYER_ATTACK3_R06)
			{
				if (iSpriteNow >= ePLAYER_ATTACK3_R04)
					m_bEffectStart = TRUE;

			}
			else if (iSpriteNow >= ePLAYER_ATTACK2_R01 && iSpriteNow <= ePLAYER_ATTACK2_R04)
			{
				if (iSpriteNow >= ePLAYER_ATTACK2_R02)
					m_bEffectStart = TRUE;
			}
			else
			{
				if (iSpriteNow >= ePLAYER_ATTACK1_R02)
					m_bEffectStart = TRUE;
			}
		}

		if (m_bEffectStart == TRUE)
			NextFrame();
	}
}