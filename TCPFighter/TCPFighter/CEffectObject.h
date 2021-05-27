#ifndef __EFFECT_OBJECT_H__
#define __EFFECT_OBJECT_H__

class CEffectObject : public CBaseObject
{
public:
	CEffectObject(DWORD objectID, int objectType, USHORT ushX, USHORT ushY, DWORD attackID);
	~CEffectObject();

	BOOL Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	void Run();

private:
	bool m_bEffectStart;
	DWORD m_dwAttackID;
	BYTE m_byCurDir;
	CPlayerObject* m_pAttacker;
};

#endif