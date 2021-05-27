#ifndef __BASE_OBJECT_H__
#define __BASE_OBJECT_H__
// a
enum e_OBJECT_TYPE
{
    eTYPE_PLAYER, eTYPE_EFFECT
};

#define dfACTION_MOVE_LL        0
#define dfACTION_MOVE_LU        1
#define dfACTION_MOVE_UU        2
#define dfACTION_MOVE_RU        3
#define dfACTION_MOVE_RR        4
#define dfACTION_MOVE_RD        5
#define dfACTION_MOVE_DD        6
#define dfACTION_MOVE_LD        7

#define dfACTION_ATTACK1        8
#define dfACTION_ATTACK2        9
#define dfACTION_ATTACK3        10
#define dfACTION_STAND			11

#define dfDELAY_STAND	        5
#define dfDELAY_MOVE	        4
#define dfDELAY_ATTACK1	        3
#define dfDELAY_ATTACK2	        4
#define dfDELAY_ATTACK3	        4
#define dfDELAY_EFFECT	        3


#define dfRANGE_MOVE_TOP        50
#define dfRANGE_MOVE_LEFT       10
#define dfRANGE_MOVE_RIGHT      630
#define dfRANGE_MOVE_BOTTOM     470

#define LL  		            0
#define RR		                4

class CBaseObject
{
public:
    CBaseObject(DWORD objectID, int objectType, USHORT ushX, USHORT ushY);
    virtual ~CBaseObject();

    void ActionInput(DWORD dwAction);
    USHORT GetCurX();
    USHORT GetCurY();
    DWORD GetObjectID();
    int GetObjectType();
    int GetSprite();

    BOOL IsEndFrame();
    void NextFrame();

    virtual BOOL Render(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
    virtual void Run();

    void SetPosition(USHORT iX, USHORT iY);
    void SetSprite(int iSpriteStart, int iSpriteMax, int iFrameDelay);

protected:
    BOOL m_bEndFrame;

    DWORD m_dwActionInput;

    USHORT m_ushCurX;
    USHORT m_ushCurY;

    int m_iDelayCount;
    int m_iFrameDelay;

    DWORD m_dwObjectID;
    int m_iObjectType;

    int m_iSpriteEnd;
    int m_iSpriteNow;
    int m_iSpriteStart;
    int m_iSpriteMax;
};

#endif