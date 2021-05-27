#include<Windows.h>
#include "PacketStruct.h"
#include "PacketDefine.h"

void mp_MakeHeader(st_HEADER* header, BYTE byCode, BYTE bySize, BYTE byType)
{
	header->byCode = byCode;
	header->bySize = bySize;
	header->byType = byType;
}

void mp_SC_CreteMyCharacter(st_MSG_SC_CREATE_MY_CHARACTER* packet, DWORD id, BYTE dir, USHORT x, USHORT y, BYTE hp)
{
	packet->dwID = id;
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
	packet->byHP = hp;
}

void mp_SC_CreteOtherCharacter(st_MSG_SC_CREATE_MY_CHARACTER* packet, DWORD id, BYTE dir, USHORT x, USHORT y, BYTE hp)
{
	packet->dwID = id;
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
	packet->byHP = hp;
}

void mp_SC_DeleteCharacter(st_MSG_SC_DELETE_CHARACTER* packet, DWORD id)
{
	packet->dwID = id;
}

void mp_CS_MoveStart(st_MSG_CS_MOVE_START* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_SC_MoveStart(st_MSG_CS_MOVE_START* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_CS_MoveStop(st_MSG_CS_MOVE_STOP* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_SC_MoveStop(st_MSG_CS_MOVE_STOP* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_CS_Attack1(st_MSG_CS_ATTACK1* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_SC_Attack1(st_MSG_SC_ATTACK1* packet, DWORD id, BYTE dir, USHORT x, USHORT y)
{
	packet->dwID = id;
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_CS_Attack2(st_MSG_CS_ATTACK2* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_SC_Attack2(st_MSG_SC_ATTACK2* packet, DWORD id, BYTE dir, USHORT x, USHORT y)
{
	packet->dwID = id;
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_CS_Attack3(st_MSG_CS_ATTACK3* packet, BYTE dir, USHORT x, USHORT y)
{
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_SC_Attack3(st_MSG_SC_ATTACK3* packet, DWORD id, BYTE dir, USHORT x, USHORT y)
{
	packet->dwID = id;
	packet->byDir = dir;
	packet->ushX = x;
	packet->ushY = y;
}

void mp_SC_Damage(st_MSG_SC_DAMAGE* packet, DWORD attackId, DWORD damageId, BYTE damageHP)
{
	packet->attackID = attackId;
	packet->damageID = damageId;
	packet->byDamageHP = damageHP;
}