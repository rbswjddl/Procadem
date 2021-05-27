#ifndef __SPRITE_DIB_H__
#define __SPRITE_DIB_H__

enum e_SRPTIE
{
    eMap,
    ePLAYER_STAND_L01,
    ePLAYER_STAND_L02,
    ePLAYER_STAND_L03,

    ePLAYER_STAND_R01,
    ePLAYER_STAND_R02,
    ePLAYER_STAND_R03,

    ePLAYER_MOVE_L01,
    ePLAYER_MOVE_L02,
    ePLAYER_MOVE_L03,
    ePLAYER_MOVE_L04,
    ePLAYER_MOVE_L05,
    ePLAYER_MOVE_L06,
    ePLAYER_MOVE_L07,
    ePLAYER_MOVE_L08,
    ePLAYER_MOVE_L09,
    ePLAYER_MOVE_L10,
    ePLAYER_MOVE_L11,
    ePLAYER_MOVE_L12,

    ePLAYER_MOVE_R01,
    ePLAYER_MOVE_R02,
    ePLAYER_MOVE_R03,
    ePLAYER_MOVE_R04,
    ePLAYER_MOVE_R05,
    ePLAYER_MOVE_R06,
    ePLAYER_MOVE_R07,
    ePLAYER_MOVE_R08,
    ePLAYER_MOVE_R09,
    ePLAYER_MOVE_R10,
    ePLAYER_MOVE_R11,
    ePLAYER_MOVE_R12,

    ePLAYER_ATTACK1_L01,
    ePLAYER_ATTACK1_L02,
    ePLAYER_ATTACK1_L03,
    ePLAYER_ATTACK1_L04,
    ePLAYER_ATTACK1_R01,
    ePLAYER_ATTACK1_R02,
    ePLAYER_ATTACK1_R03,
    ePLAYER_ATTACK1_R04,

    ePLAYER_ATTACK2_L01,
    ePLAYER_ATTACK2_L02,
    ePLAYER_ATTACK2_L03,
    ePLAYER_ATTACK2_L04,
    ePLAYER_ATTACK2_R01,
    ePLAYER_ATTACK2_R02,
    ePLAYER_ATTACK2_R03,
    ePLAYER_ATTACK2_R04,

    ePLAYER_ATTACK3_L01,
    ePLAYER_ATTACK3_L02,
    ePLAYER_ATTACK3_L03,
    ePLAYER_ATTACK3_L04,
    ePLAYER_ATTACK3_L05,
    ePLAYER_ATTACK3_L06,
    ePLAYER_ATTACK3_R01,
    ePLAYER_ATTACK3_R02,
    ePLAYER_ATTACK3_R03,
    ePLAYER_ATTACK3_R04,
    ePLAYER_ATTACK3_R05,
    ePLAYER_ATTACK3_R06,

    eEFFECT_SPARK_01,
    eEFFECT_SPARK_02,
    eEFFECT_SPARK_03,
    eEFFECT_SPARK_04,

    eGAUGE_HP,
    eSHADOW
};

class CSpriteDib
{
public:
	typedef struct st_SPRITE
	{
		BYTE* bypImage;
		int			iWidth;
		int			iHeight;
		int			iPitch;

		int			iCenterPointX;
		int			iCenterPointY;
	};

	CSpriteDib(int iMaxSprite, DWORD dwColorKey);
	virtual ~CSpriteDib();

	BOOL LoadDibSprite(int iSpriteIndex, const WCHAR* szFileName, int iCenterPointX, int iCenterPointY);
	void ReleaseSprite(int iSpriteIndex);

	void DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY,
		BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);

	void DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY,
		BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);

	void DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY,
		BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);

	void DrawImage(int iSpriteIndex, int iDrawX, int iDrawY,
		BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen = 100);

protected:
	int			m_iMaxSprite;
	st_SPRITE* m_stpSprite;
	DWORD		m_dwColorKey;
};

#endif