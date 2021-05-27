#pragma once

#ifndef __PACKET_STRUCT_H__
#define __PACKET_STRUCT_H__

// 안했을 때 구조체 포인터 접근이 어긋나는 경우 발생

#pragma pack(1)
// PACKET STRUCT

typedef struct
{
	BYTE	byCode;			// 패킷코드 0x89 고정.
	BYTE	bySize;			// 패킷 사이즈.
	BYTE	byType;			// 패킷타입.
}st_HEADER;

typedef struct
{
	DWORD	dwID;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
	BYTE	byHP;
}st_MSG_SC_CREATE_MY_CHARACTER;

typedef struct
{
	DWORD	dwID;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
	BYTE	byHP;
}st_MSG_SC_CREATE_OTHER_CHARACTER;

typedef struct
{
	DWORD	dwID;
}st_MSG_SC_DELETE_CHARACTER;

typedef struct
{
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_CS_MOVE_START;

typedef struct
{
	DWORD	dwId;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_SC_MOVE_START;

typedef struct
{
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_CS_MOVE_STOP;

typedef struct
{
	DWORD	dwID;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_SC_MOVE_STOP;

typedef struct
{
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_CS_ATTACK1;

typedef struct
{
	DWORD	dwID;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_SC_ATTACK1;

typedef struct
{
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_CS_ATTACK2;

typedef struct
{
	DWORD	dwID;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_SC_ATTACK2;

typedef struct
{
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_CS_ATTACK3;

typedef struct
{
	DWORD	dwID;
	BYTE	byDir;
	USHORT	ushX;
	USHORT	ushY;
}st_MSG_SC_ATTACK3;

typedef struct
{
	DWORD	attackID;
	DWORD	damageID;
	BYTE	byDamageHP;
}st_MSG_SC_DAMAGE;

// Make Packet Function
void mp_MakeHeader(st_HEADER* header, BYTE byCode, BYTE bySize, BYTE byType);

void mp_SC_CreteMyCharacter(st_MSG_SC_CREATE_MY_CHARACTER* packet, DWORD id, BYTE dir, USHORT x, USHORT y, BYTE hp);
void mp_SC_CreteOtherCharacter(st_MSG_SC_CREATE_MY_CHARACTER* packet, DWORD id, BYTE dir, USHORT x, USHORT y, BYTE hp);

void mp_SC_DeleteCharacter(st_MSG_SC_DELETE_CHARACTER* packet, DWORD id);

void mp_CS_MoveStart(st_MSG_CS_MOVE_START* packet, BYTE dir, USHORT x, USHORT y);
void mp_SC_MoveStart(st_MSG_CS_MOVE_START* packet, BYTE dir, USHORT x, USHORT y);

void mp_CS_MoveStop(st_MSG_CS_MOVE_STOP* packet, BYTE dir, USHORT x, USHORT y);
void mp_SC_MoveStop(st_MSG_CS_MOVE_STOP* packet, BYTE dir, USHORT x, USHORT y);

void mp_CS_Attack1(st_MSG_CS_ATTACK1* packet, BYTE dir, USHORT x, USHORT y);
void mp_SC_Attack1(st_MSG_SC_ATTACK1* packet, DWORD id, BYTE dir, USHORT x, USHORT y);

void mp_CS_Attack2(st_MSG_CS_ATTACK2* packet, BYTE dir, USHORT x, USHORT y);
void mp_SC_Attack2(st_MSG_SC_ATTACK2* packet, DWORD id, BYTE dir, USHORT x, USHORT y);

void mp_CS_Attack3(st_MSG_CS_ATTACK3* packet, BYTE dir, USHORT x, USHORT y);
void mp_SC_Attack3(st_MSG_SC_ATTACK3* packet, DWORD id, BYTE dir, USHORT x, USHORT y);

void mp_SC_Damage(st_MSG_SC_DAMAGE* packet, DWORD attackId, DWORD damageId, BYTE damageHP);

// 안하면 동적할당 해제할 때 heap corruption 발생
#pragma pack()
#endif