#ifndef __PLAYER_OBJECT_H__
#define __PLAYER_OBJECT_H__
// a
class CPlayerObject : public CBaseObject
{
public:
	CPlayerObject(BOOL bPlayer, DWORD objectID, int objectType, USHORT iX, USHORT iY, BYTE iDir, BYTE hp);
	~CPlayerObject();

	BOOL IsPlayer();

	void ActionProc();
	void InputActionProc();

	BOOL Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	void Run();

	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionMove();
	void SetActionStand();
	void SetDirection(int iDir);
	void SetHP(char chHP);

	DWORD GetDirection();
	char GetHP();

private:
	void CheckDoMove(BYTE byDir);
	void CheckDoStand(BYTE byDir);
	void CheckDoAttack1();
	void CheckDoAttack2();
	void CheckDoAttack3();

protected:
	BOOL	m_bPlayerCharacter;
	BYTE	m_chHP;
	DWORD	m_dwActionCur;
	DWORD	m_dwActionOld;
	USHORT	m_iDirCur;
	USHORT	m_iDirOld;
};

#endif