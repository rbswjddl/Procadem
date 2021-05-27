#include<Windows.h>
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"
#include "CSpriteDib.h"
#include "LinkedList.h"
#include "PacketDefine.h"
#include "PacketStruct.h"
#include "CRingBuffer.h"
// a
// 네트워크용 전역변수
typedef struct st_SESSION
{
	st_SESSION()
	{
		servSock = INVALID_SOCKET;
		bConnect = FALSE;
	}
	SOCKET servSock;
	CRingBuffer recvQ;
	CRingBuffer sendQ;
	BOOL bConnect;
}st_SESSION;

extern st_SESSION g_MySession;
extern CSpriteDib g_SpriteDib;
extern CList<CBaseObject*> g_List;

CPlayerObject::CPlayerObject(BOOL bPlayer, DWORD objectID, int objectType, USHORT iX, USHORT iY, BYTE iDir, BYTE hp)
	:m_bPlayerCharacter(bPlayer), CBaseObject(objectID, objectType, iX, iY)
{
	m_dwActionOld = dfACTION_STAND;
	m_dwActionCur = dfACTION_STAND;

	m_chHP = hp;

	m_iDirCur = iDir;
	m_iDirOld = iDir;
}

CPlayerObject::~CPlayerObject()
{

}

void CPlayerObject::ActionProc()
{
	if (IsPlayer())
	{
		switch (m_dwActionCur)
		{
		case dfACTION_ATTACK1:
		case dfACTION_ATTACK2:
		case dfACTION_ATTACK3:

			if (IsEndFrame())
			{
				SetActionStand();

				m_dwActionInput = dfACTION_STAND; // 내 화면에서 다른 플레이어 공격 후 원상복귀
				m_dwActionCur = dfACTION_STAND;   // 내 캐릭터 공격후 움직일 수 있게
			}
			break;

		default:
			InputActionProc();
			break;
		}
	}
	else
	{
		switch (m_dwActionInput)
		{
		case dfACTION_ATTACK1:
		case dfACTION_ATTACK2:
		case dfACTION_ATTACK3:

			if (IsEndFrame())
			{
				SetActionStand();

				m_dwActionInput = dfACTION_STAND; // 내 화면에서 다른 플레이어 공격 후 원상복귀
				m_dwActionCur = dfACTION_STAND;   // 내 캐릭터 공격후 움직일 수 있게
			}
		default:
			InputActionProc();
		}
	}

}

void CPlayerObject::InputActionProc()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = m_dwActionInput;

	USHORT oldX = GetCurX();
	USHORT oldY = GetCurY();


	// STAND: 0, MOVE:1, ATTACK: 2

	switch (m_dwActionCur)
	{
	case dfACTION_MOVE_LL:
		SetDirection(LL);
		//전에 행동과 현재의 행동이 같은데 SetAction 해주면 움직임이 고정된다.


		CheckDoMove(dfACTION_MOVE_LL);

		m_ushCurX -= 3;
		break;

	case dfACTION_MOVE_LU:
		SetDirection(LL);
		CheckDoMove(dfACTION_MOVE_LU);

		m_ushCurX -= 3;
		m_ushCurY -= 2;
		break;

	case dfACTION_MOVE_UU:
		CheckDoMove(dfACTION_MOVE_UU);

		m_ushCurY -= 2;
		break;

	case dfACTION_MOVE_RU:
		SetDirection(RR);
		CheckDoMove(dfACTION_MOVE_RU);

		m_ushCurX += 3;
		m_ushCurY -= 2;
		break;

	case dfACTION_MOVE_RR:
		SetDirection(RR);
		CheckDoMove(dfACTION_MOVE_RR);

		m_ushCurX += 3;
		break;

	case dfACTION_MOVE_RD:
		SetDirection(RR);
		CheckDoMove(dfACTION_MOVE_RD);

		m_ushCurX += 3;
		m_ushCurY += 2;
		break;

	case dfACTION_MOVE_DD:
		CheckDoMove(dfACTION_MOVE_DD);

		m_ushCurY += 2;
		break;

	case dfACTION_MOVE_LD:
		SetDirection(LL);
		CheckDoMove(dfACTION_MOVE_LD);

		m_ushCurX -= 3;
		m_ushCurY += 2;
		break;

	case dfACTION_ATTACK1:
		CheckDoAttack1();
		break;

	case dfACTION_ATTACK2:
		CheckDoAttack2();
		break;

	case dfACTION_ATTACK3:
		CheckDoAttack3();
		break;

	case dfACTION_STAND:

		CheckDoStand(dfACTION_STAND);
		break;
	}

	// 이렇게 좌표 다 이동시켜놓고 마지막에 범위처리 들어가야지!!!
	if (m_ushCurX <= dfRANGE_MOVE_LEFT)
	{
		m_ushCurX = dfRANGE_MOVE_LEFT;
		m_ushCurY = oldY;
	}

	if (m_ushCurX >= dfRANGE_MOVE_RIGHT)
	{
		m_ushCurX = dfRANGE_MOVE_RIGHT;
		m_ushCurY = oldY;
	}

	if (m_ushCurY <= dfRANGE_MOVE_TOP)
	{
		m_ushCurX = oldX;
		m_ushCurY = dfRANGE_MOVE_TOP;
	}

	if (m_ushCurY >= dfRANGE_MOVE_BOTTOM)
	{
		m_ushCurX = oldX;
		m_ushCurY = dfRANGE_MOVE_BOTTOM;
	}
}

DWORD CPlayerObject::GetDirection()
{
	return m_iDirCur;
}

char CPlayerObject::GetHP()
{
	return m_chHP;
}

void CPlayerObject::CheckDoMove(BYTE byDir)
{
	st_HEADER stHeader;
	st_MSG_CS_MOVE_START stMoveStart;

	if (m_dwActionOld != m_dwActionCur)
	{
		if (IsPlayer())
		{
			mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_MOVE_START), dfPACKET_CS_MOVE_START);
			mp_CS_MoveStart(&stMoveStart, byDir, m_ushCurX, m_ushCurY);
			g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
			g_MySession.sendQ.Enqueue((char*)&stMoveStart, sizeof(st_MSG_CS_MOVE_START));
		}

		if (m_dwActionOld == dfACTION_STAND || m_iDirOld != m_iDirCur)
			SetActionMove();
	}
}

void CPlayerObject::CheckDoStand(BYTE byDir)
{
	st_HEADER stHeader;
	st_MSG_CS_MOVE_STOP stMoveStop;

	if (m_dwActionCur != m_dwActionOld)
	{
		if (IsPlayer())
		{
			mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_MOVE_STOP), dfPACKET_CS_MOVE_STOP);
			mp_CS_MoveStop(&stMoveStop, m_iDirCur, m_ushCurX, m_ushCurY);

			g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
			g_MySession.sendQ.Enqueue((char*)&stMoveStop, sizeof(st_MSG_CS_MOVE_STOP));
		}

		SetActionStand();
	}
}

void CPlayerObject::CheckDoAttack1()
{
	st_HEADER stHeader;
	st_MSG_CS_MOVE_STOP stMoveStop;
	st_MSG_CS_ATTACK1 stAttack1;

	if (m_dwActionOld != m_dwActionCur)
	{
		if (IsPlayer())
		{
			if (m_dwActionOld == dfACTION_MOVE_UU || m_dwActionOld == dfACTION_MOVE_RU ||
				m_dwActionOld == dfACTION_MOVE_RR || m_dwActionOld == dfACTION_MOVE_RD ||
				m_dwActionOld == dfACTION_MOVE_DD || m_dwActionOld == dfACTION_MOVE_LD ||
				m_dwActionOld == dfACTION_MOVE_LL || m_dwActionOld == dfACTION_MOVE_LU)
			{
				mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_MOVE_STOP), dfPACKET_CS_MOVE_STOP);
				mp_CS_MoveStop(&stMoveStop, m_iDirCur, m_ushCurX, m_ushCurY);

				g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
				g_MySession.sendQ.Enqueue((char*)&stMoveStop, sizeof(st_MSG_CS_MOVE_STOP));
			}

			mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_ATTACK1), dfPACKET_CS_ATTACK1);
			mp_CS_Attack1(&stAttack1, m_iDirCur, m_ushCurX, m_ushCurY);

			g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
			g_MySession.sendQ.Enqueue((char*)&stAttack1, sizeof(st_MSG_CS_ATTACK1));

		}

		SetActionAttack1();
	}
}

void CPlayerObject::CheckDoAttack2()
{
	st_HEADER stHeader;
	st_MSG_CS_MOVE_STOP stMoveStop;
	st_MSG_CS_ATTACK2 stAttack2;

	if (m_dwActionOld != m_dwActionCur)
	{
		if (IsPlayer())
		{
			if (m_dwActionOld == dfACTION_MOVE_UU || m_dwActionOld == dfACTION_MOVE_RU ||
				m_dwActionOld == dfACTION_MOVE_RR || m_dwActionOld == dfACTION_MOVE_RD ||
				m_dwActionOld == dfACTION_MOVE_DD || m_dwActionOld == dfACTION_MOVE_LD ||
				m_dwActionOld == dfACTION_MOVE_LL || m_dwActionOld == dfACTION_MOVE_LU)
			{
				mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_MOVE_STOP), dfPACKET_CS_MOVE_STOP);
				mp_CS_MoveStop(&stMoveStop, m_iDirCur, m_ushCurX, m_ushCurY);

				g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
				g_MySession.sendQ.Enqueue((char*)&stMoveStop, sizeof(st_MSG_CS_MOVE_STOP));
			}

			mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_ATTACK2), dfPACKET_CS_ATTACK2);
			mp_CS_Attack2(&stAttack2, m_iDirCur, m_ushCurX, m_ushCurY);

			g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
			g_MySession.sendQ.Enqueue((char*)&stAttack2, sizeof(st_MSG_CS_ATTACK2));

		}
		SetActionAttack2();
	}
}

void CPlayerObject::CheckDoAttack3()
{
	st_HEADER stHeader;
	st_MSG_CS_MOVE_STOP stMoveStop;
	st_MSG_CS_ATTACK3 stAttack3;

	if (m_dwActionOld != m_dwActionCur)
	{
		if (IsPlayer())
		{
			if (m_dwActionOld == dfACTION_MOVE_UU || m_dwActionOld == dfACTION_MOVE_RU ||
				m_dwActionOld == dfACTION_MOVE_RR || m_dwActionOld == dfACTION_MOVE_RD ||
				m_dwActionOld == dfACTION_MOVE_DD || m_dwActionOld == dfACTION_MOVE_LD ||
				m_dwActionOld == dfACTION_MOVE_LL || m_dwActionOld == dfACTION_MOVE_LU)
			{
				mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_MOVE_STOP), dfPACKET_CS_MOVE_STOP);
				mp_CS_MoveStop(&stMoveStop, m_iDirCur, m_ushCurX, m_ushCurY);

				g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
				g_MySession.sendQ.Enqueue((char*)&stMoveStop, sizeof(st_MSG_CS_MOVE_STOP));
			}

			mp_MakeHeader(&stHeader, 0x89, sizeof(st_MSG_CS_ATTACK3), dfPACKET_CS_ATTACK3);
			mp_CS_Attack3(&stAttack3, m_iDirCur, m_ushCurX, m_ushCurY);

			g_MySession.sendQ.Enqueue((char*)&stHeader, sizeof(st_HEADER));
			g_MySession.sendQ.Enqueue((char*)&stAttack3, sizeof(st_MSG_CS_ATTACK3));

		}

		SetActionAttack3();
	}
}

BOOL CPlayerObject::IsPlayer()
{
	return m_bPlayerCharacter;
}

BOOL CPlayerObject::Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	g_SpriteDib.DrawSprite50(eSHADOW, m_ushCurX, m_ushCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	// Red로 찍으면 전부 Red 되는데 원인 파악해보자!! -> *sprite는 건드는거 아니다.
	if (IsPlayer())
		g_SpriteDib.DrawSpriteRed(GetSprite(), m_ushCurX, m_ushCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	else
		g_SpriteDib.DrawSprite(GetSprite(), m_ushCurX, m_ushCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);

	g_SpriteDib.DrawImage(eGAUGE_HP, m_ushCurX - 35, m_ushCurY + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, m_chHP);
	return TRUE;
}

void CPlayerObject::Run()
{
	NextFrame();
	ActionProc();
}

void CPlayerObject::SetActionAttack1()
{
	if (m_iDirCur == LL)
		SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L04, dfDELAY_ATTACK1);
	else
		SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R04, dfDELAY_ATTACK1);
}

void CPlayerObject::SetActionAttack2()
{
	if (m_iDirCur == LL)
		SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L04, dfDELAY_ATTACK2);
	else
		SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R04, dfDELAY_ATTACK2);
}

void CPlayerObject::SetActionAttack3()
{
	if (m_iDirCur == LL)
		SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L06, dfDELAY_ATTACK3);
	else
		SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R06, dfDELAY_ATTACK3);
}

void CPlayerObject::SetActionMove()
{
	if (m_iDirCur == LL)
		SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L12, dfDELAY_MOVE);
	else
		SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R12, dfDELAY_MOVE);
}

void CPlayerObject::SetActionStand()
{
	if (m_iDirCur == LL)
		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, dfDELAY_STAND);
	else
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R03, dfDELAY_STAND);
}

void CPlayerObject::SetDirection(int iDir)
{
	m_iDirOld = m_iDirCur;
	m_iDirCur = iDir;
}

void CPlayerObject::SetHP(char chHP)
{
	m_chHP = chHP;
}