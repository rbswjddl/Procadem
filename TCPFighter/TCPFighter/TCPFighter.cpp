// TCPFighter.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// 수정 사항2: 공격이 씹히지 않게끔 하는거 추가해야된다.

// 질문 사항1:  
// attackID 대신에 그냥 attack 객체를 effect 객체 만들 때 넘겨주는 형식
// 을 사용하면 종속성이 더 높아져서 문제가 생기는 건가?? => 질문
// 객체의 생성과 삭제의 위치가 달라지는데.... 어떻게 해야하는가??

#include<WinSock2.h>
#include<WS2tcpip.h>
#include "PacketDefine.h"
#include "PacketStruct.h"
#include "framework.h"
#include "TCPFighter.h"
#include "CScreenDib.h"
#include "CSpriteDib.h"
#include "CBaseObject.h"
#include "CPlayerObject.h"
#include "CEffectObject.h"
#include "LinkedList.h"
#include "CRingBuffer.h"
#include "CPacket.h"
#include<timeapi.h>
//#include "CFrameSkip.h"
#include<windowsx.h>

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "ws2_32.lib")

#define MAX_LOADSTRING      100
#define WM_NETWORK          (WM_USER + 1)

// Network 함수 구별
#define SERVERTOCLIENT      0
#define CLIENTTOSERVER      1


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

//Network Session Struct
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


// 내가 선언한 전역변수
////////////////////////////////////////////////////
HWND g_hWnd;
BOOL g_bActiveApp;
HIMC g_hOldIMC;

DWORD g_LogicFrameOldCnt = 0;
DWORD g_LogicFrameCnt = 0;

DWORD g_RenderFrameOldCnt = 0;
DWORD g_RenderFrameCnt = 0;

DWORD g_maxFrameCnt = 50;
DWORD g_FrameCnt;

CScreenDib g_ScreenDib(640, 480, 32);
CSpriteDib g_SpriteDib(100, 0x00ffffff);

CList<CBaseObject*> g_List;
CBaseObject* g_pPlayerObject;

// Network Global Variable
st_SESSION g_MySession;

// 게임 관련된 전연함수
BOOL InitGame(void);
void Update();
void KeyProcess();
void Run();
void Render();
void HeightSort();
void TimeCheckFunc();

// Network Global Function
void InitSocket(HWND hWnd);
void ReadEvent();
void SendEvent();
void PacketProcCreateMyCharater(CPacket*);
void PacketProcCreateOtherCharater(CPacket*);
void PacketProcDeleteCharater(CPacket*);
void PacketProcSCMoveStart(CPacket*);
void PacketProcSCMoveStop(CPacket*);
void PacketProcSCAttack1(CPacket*);
void PacketProcSCAttack2(CPacket*);
void PacketProcSCAttack3(CPacket*);
void PacketProcDamage(CPacket*);

void SendPacket();

////////////////////////////////////////////////////

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    timeBeginPeriod(1);
    InitGame();

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCPFIGHTER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Assignment";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(L"Assignment", L"TCPFighter", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);

    g_hWnd = hWnd;

    RECT winRect;
    winRect.top = 0;
    winRect.left = 0;
    winRect.right = 640;
    winRect.bottom = 480;

    AdjustWindowRectEx(&winRect, GetWindowStyle(g_hWnd), GetMenu(g_hWnd) != NULL, GetWindowExStyle(g_hWnd));

    int iX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (640 / 2);
    int iY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (480 / 2);

    MoveWindow(g_hWnd, iX, iY, winRect.right - winRect.left, winRect.bottom - winRect.top, TRUE);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TCPFIGHTER));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (1)
    {
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
        }
    }

    timeEndPeriod(1);
    return (int)msg.wParam;
}


BOOL InitGame(void)
{
    if (!g_SpriteDib.LoadDibSprite(eMap, L"SpriteData\\_Map.bmp", 0, 0)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L01, L"SpriteData\\Stand_L_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L02, L"SpriteData\\Stand_L_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_L03, L"SpriteData\\Stand_L_03.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R01, L"SpriteData\\Stand_R_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R02, L"SpriteData\\Stand_R_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_STAND_R03, L"SpriteData\\Stand_R_03.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L01, L"SpriteData\\Move_L_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L02, L"SpriteData\\Move_L_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L03, L"SpriteData\\Move_L_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L04, L"SpriteData\\Move_L_04.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L05, L"SpriteData\\Move_L_05.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L06, L"SpriteData\\Move_L_06.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L07, L"SpriteData\\Move_L_07.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L08, L"SpriteData\\Move_L_08.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L09, L"SpriteData\\Move_L_09.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L10, L"SpriteData\\Move_L_10.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L11, L"SpriteData\\Move_L_11.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_L12, L"SpriteData\\Move_L_12.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R01, L"SpriteData\\Move_R_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R02, L"SpriteData\\Move_R_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R03, L"SpriteData\\Move_R_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R04, L"SpriteData\\Move_R_04.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R05, L"SpriteData\\Move_R_05.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R06, L"SpriteData\\Move_R_06.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R07, L"SpriteData\\Move_R_07.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R08, L"SpriteData\\Move_R_08.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R09, L"SpriteData\\Move_R_09.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R10, L"SpriteData\\Move_R_10.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R11, L"SpriteData\\Move_R_11.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_MOVE_R12, L"SpriteData\\Move_R_12.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L01, L"SpriteData\\Attack1_L_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L02, L"SpriteData\\Attack1_L_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L03, L"SpriteData\\Attack1_L_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_L04, L"SpriteData\\Attack1_L_04.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R01, L"SpriteData\\Attack1_R_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R02, L"SpriteData\\Attack1_R_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R03, L"SpriteData\\Attack1_R_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK1_R04, L"SpriteData\\Attack1_R_04.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L01, L"SpriteData\\Attack2_L_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L02, L"SpriteData\\Attack2_L_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L03, L"SpriteData\\Attack2_L_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_L04, L"SpriteData\\Attack2_L_04.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R01, L"SpriteData\\Attack2_R_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R02, L"SpriteData\\Attack2_R_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R03, L"SpriteData\\Attack2_R_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK2_R04, L"SpriteData\\Attack2_R_04.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L01, L"SpriteData\\Attack3_L_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L02, L"SpriteData\\Attack3_L_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L03, L"SpriteData\\Attack3_L_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L04, L"SpriteData\\Attack3_L_04.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L05, L"SpriteData\\Attack3_L_05.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_L06, L"SpriteData\\Attack3_L_06.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R01, L"SpriteData\\Attack3_R_01.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R02, L"SpriteData\\Attack3_R_02.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R03, L"SpriteData\\Attack3_R_03.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R04, L"SpriteData\\Attack3_R_04.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R05, L"SpriteData\\Attack3_R_05.bmp", 71, 90)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(ePLAYER_ATTACK3_R06, L"SpriteData\\Attack3_R_06.bmp", 71, 90)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_01, L"SpriteData\\xSpark_1.bmp", 70, 70)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_04, L"SpriteData\\xSpark_4.bmp", 70, 70)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_02, L"SpriteData\\xSpark_2.bmp", 70, 70)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(eEFFECT_SPARK_03, L"SpriteData\\xSpark_3.bmp", 70, 70)) return FALSE;

    if (!g_SpriteDib.LoadDibSprite(eGAUGE_HP, L"SpriteData\\HPGuage.bmp", 0, 0)) return FALSE;
    if (!g_SpriteDib.LoadDibSprite(eSHADOW, L"SpriteData\\Shadow.bmp", 32, 4)) return FALSE;
    return TRUE;
}

void KeyProcess()
{
    DWORD dwAction;

    if (g_pPlayerObject == NULL)
        return;

    if (GetAsyncKeyState(0x41))
    {
        dwAction = dfACTION_ATTACK1;
    }
    else if (GetAsyncKeyState(0x53))
    {
        dwAction = dfACTION_ATTACK2;
    }
    else if (GetAsyncKeyState(0x44))
    {
        dwAction = dfACTION_ATTACK3;
    }
    else if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_UP))
    {
        dwAction = dfACTION_MOVE_LU;
    }
    else if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_UP))
    {
        dwAction = dfACTION_MOVE_RU;
    }
    else if (GetAsyncKeyState(VK_LEFT) && GetAsyncKeyState(VK_DOWN))
    {
        dwAction = dfACTION_MOVE_LD;
    }
    else if (GetAsyncKeyState(VK_RIGHT) && GetAsyncKeyState(VK_DOWN))
    {
        dwAction = dfACTION_MOVE_RD;
    }
    else if (GetAsyncKeyState(VK_LEFT))
    {
        dwAction = dfACTION_MOVE_LL;
    }
    else if (GetAsyncKeyState(VK_UP))
    {
        dwAction = dfACTION_MOVE_UU;
    }
    else if (GetAsyncKeyState(VK_RIGHT))
    {
        dwAction = dfACTION_MOVE_RR;
    }
    else if (GetAsyncKeyState(VK_DOWN))
    {
        dwAction = dfACTION_MOVE_DD;
    }
    else
    {
        dwAction = dfACTION_STAND;
    }

    g_pPlayerObject->ActionInput(dwAction);
}

void HeightSort()
{
    int iSize = g_List.size();

    for (int i = 0; i < iSize - 1; i++)
    {
        auto iter = g_List.begin();
        for (int j = 0; j < iSize - 1 - i; j++)
        {
            if (((CBaseObject*)(*iter)->GetCurY() > ((CBaseObject*)(*(iter + 1))->GetCurY())))
            {
                g_List.swap(iter, (iter + 1));
            }
            else
                iter++;
        }
    }
}

void Run()
{
    SendPacket();
    auto iter = g_List.begin();

    for (; iter != g_List.end();)
    {
        (*iter)->Run();
        if ((*iter)->GetObjectType() == eTYPE_EFFECT)
        {
            if ((*iter)->IsEndFrame())
            {
                delete* iter;
                iter = g_List.erase(iter);
            }
            else
                ++iter;
        }
        else
            ++iter;
    }

    //정렬
    HeightSort();

    // Effect 제일 뒤로
    int iSize = g_List.size();
    iter = g_List.begin();
    for (int i = 0; i < iSize; i++)
    {
        if ((*iter)->GetObjectType() == eTYPE_EFFECT)
        {
            g_List.push_back((*iter));
            iter = g_List.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    g_LogicFrameCnt++;
}

void Render()
{
    BYTE* bypDest = g_ScreenDib.GetDibBuffer();
    int iDestWidth = g_ScreenDib.GetWidth();
    int iDestHeight = g_ScreenDib.GetHeight();
    int iDestPitch = g_ScreenDib.GetPitch();

    g_SpriteDib.DrawImage(eMap, 0, 0, bypDest, iDestWidth, iDestHeight, iDestPitch);

    auto iter = g_List.begin();
    for (; iter != g_List.end(); ++iter)
        (*iter)->Render(bypDest, iDestWidth, iDestHeight, iDestPitch);

    g_RenderFrameCnt++;
}

void TimeCheckFunc()
{
    static DWORD dwCurTicks;
    static DWORD dwOldTicks;
    static DWORD dwElaspedTicks;

    dwCurTicks = timeGetTime();

    if (dwOldTicks == 0)
    {
        dwElaspedTicks = 0;
    }
    else
    {
        dwElaspedTicks += dwCurTicks - dwOldTicks;
    }

    if (dwElaspedTicks >= 1000)
    {
        dwElaspedTicks -= 1000;
        WCHAR buff[30];
        wsprintf(buff, L"TCP Fighter LPS:%d RPS:%d", g_LogicFrameCnt, g_RenderFrameCnt);
        SetWindowTextW(g_hWnd, buff);

        wprintf(L"Logic Frame: %d Render Frame: %d\n", g_LogicFrameCnt, g_RenderFrameCnt);
        g_LogicFrameCnt = 0;
        g_RenderFrameCnt = 0;
    }

    dwOldTicks = timeGetTime();
}

void Update()
{
    static DWORD dwCurTicks;
    static DWORD dwOldTicks;
    static DWORD dwExtraTicks;
    DWORD sub = 0;
    DWORD dwElaspedTicks;

    if (g_bActiveApp)
        KeyProcess();

    Run();

    dwCurTicks = timeGetTime();

    if (dwOldTicks == 0)
    {
        dwElaspedTicks = 0;
    }
    else
    {
        dwElaspedTicks = dwCurTicks - dwOldTicks;
    }


    if (dwElaspedTicks > 20)
    {
        dwExtraTicks += (dwElaspedTicks - 20);
    }

    if (dwExtraTicks >= 20)
    {
        // 랜더 스킵
        // 슬립 스킵
        if (dwElaspedTicks <= 20)
        {
            dwExtraTicks -= (20 - dwElaspedTicks);
        }

        dwOldTicks = timeGetTime();
    }
    else
    {
        if (dwElaspedTicks > 20)
        {
            // 슬립 스킵
            Render();
            g_ScreenDib.DrawBuffer(g_hWnd);
            dwOldTicks = timeGetTime() - (dwElaspedTicks * (dwElaspedTicks / 20));
        }
        else
        {
            // 정상 동작
            Render();
            g_ScreenDib.DrawBuffer(g_hWnd);
            //DWORD a = timeGetTime();
            Sleep(20 - dwElaspedTicks);
            //DWORD b = timeGetTime();
            //sub = (b - a) - (20 - dwElaspedTicks);

            dwOldTicks = dwCurTicks + 20 - dwElaspedTicks;
        }
    }

    //dwOldTicks -= sub;
    TimeCheckFunc();
}

void InitSocket(HWND hWnd)
{
    WSADATA wsaData;
    SOCKADDR_IN servAdr;
    DWORD errorCode;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        wprintf(L"WSAStartup error: %d\n", WSAGetLastError());
        exit(1);
    }

    g_MySession.servSock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_port = htons(5000);
    InetPtonW(AF_INET, L"127.0.0.1", &servAdr.sin_addr);

    WSAAsyncSelect(g_MySession.servSock, hWnd, WM_NETWORK, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);

    if (connect(g_MySession.servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    {
        errorCode = WSAGetLastError();
        if (errorCode != WSAEWOULDBLOCK)
        {
            wprintf(L"Connect Error: %d\n", errorCode);
            return;
        }
    }
}

void ReadEvent()
{
    //char buffer[500];
    CPacket clPacket;
    DWORD errCode;
    st_HEADER stHeader;
    //int readByte = recv(g_MySession.servSock, buffer, sizeof(buffer), 0);

    int readByte = recv(g_MySession.servSock, clPacket.GetBufferPtr(), clPacket.GetBufferSize(), 0);

    if (readByte == 0 || readByte == SOCKET_ERROR)
    {
        errCode = WSAGetLastError();
        if (errCode != WSAEWOULDBLOCK)
        {
            wprintf(L"ReadEvent::recv Error: %d\n", errCode);
            exit(1);
        }
    }

    // Recv Queue Error
    int iFreeSize = g_MySession.recvQ.GetFreeSize();
    if (iFreeSize < readByte)
    {
        wprintf(L"My Recv Queue Error!!\n");
        exit(1);
    }

    //g_MySession.recvQ.Enqueue(buffer, readByte);
    g_MySession.recvQ.Enqueue(clPacket.GetBufferPtr(), readByte);
    clPacket.MoveWritePos(readByte);

    while (TRUE)
    {
        if (g_MySession.recvQ.GetUseSize() < sizeof(st_HEADER))
            break;

        g_MySession.recvQ.Peek(reinterpret_cast<char*>(&stHeader), sizeof(st_HEADER));

        if (stHeader.byCode != 0X89)
            break;

        if (g_MySession.recvQ.GetUseSize() < sizeof(st_HEADER) + stHeader.bySize)
            break;

        g_MySession.recvQ.MoveFront(sizeof(st_HEADER));
       

        //g_MySession.recvQ.Dequeue(buffer, stHeader.bySize);
        g_MySession.recvQ.Dequeue(clPacket.GetBufferPtr(), stHeader.bySize);

        switch (stHeader.byType)
        {
        case dfPACKET_SC_CREATE_MY_CHARACTER:
            PacketProcCreateMyCharater(&clPacket);
            break;
        case dfPACKET_SC_CREATE_OTHER_CHARACTER:
            PacketProcCreateOtherCharater(&clPacket);
            break;
        case dfPACKET_SC_DELETE_CHARACTER:
            PacketProcDeleteCharater(&clPacket);
            break;
        case dfPACKET_SC_MOVE_START:
            PacketProcSCMoveStart(&clPacket);
            break;
        case dfPACKET_SC_MOVE_STOP:
            PacketProcSCMoveStop(&clPacket);
            break;
        case dfPACKET_SC_ATTACK1:
            PacketProcSCAttack1(&clPacket);
            break;
        case dfPACKET_SC_ATTACK2:
            PacketProcSCAttack2(&clPacket);
            break;
        case dfPACKET_SC_ATTACK3:
            PacketProcSCAttack3(&clPacket);
            break;
        case dfPACKET_SC_DAMAGE:
            PacketProcDamage(&clPacket);
            break;
        default:
            return;
        }
    }
}

void PacketProcCreateMyCharater(CPacket* clpPacket)
{
    unsigned int dwID;
    BYTE byDir;
    USHORT x;
    USHORT y;
    BYTE hp;

    *clpPacket >> dwID >> byDir >> x >> y >> hp;

    CBaseObject* player = new CPlayerObject(TRUE, dwID, eTYPE_PLAYER, x, y, byDir, hp);
    g_List.push_back(player);

    g_pPlayerObject = player;
    g_pPlayerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, dfDELAY_STAND);
}

void PacketProcCreateOtherCharater(CPacket* clpPacket)
{
    // client 못 믿어 무조건 예외처리 해야되는거야!!

    unsigned int dwID;
    BYTE byDir;
    USHORT x;
    USHORT y;
    BYTE hp;

    *clpPacket >> dwID >> byDir >> x >> y >> hp;

    if (byDir != LL && byDir == RR)
    {
        //예외 처리
    }

    if (x < dfRANGE_MOVE_LEFT || x > dfRANGE_MOVE_RIGHT)
    {

    }

    if (y < dfRANGE_MOVE_BOTTOM || y > dfRANGE_MOVE_TOP)
    {

    }

    if (hp > 100 || hp < 0)
    {

    }

    CBaseObject* pEnemy = new CPlayerObject(FALSE, dwID, eTYPE_PLAYER, x, y, byDir, hp);
    if (byDir == LL)
        pEnemy->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, dfDELAY_STAND);
    else
        pEnemy->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R03, dfDELAY_STAND);
    pEnemy->ActionInput(dfACTION_STAND);

    g_List.push_back(pEnemy);
}

void PacketProcDeleteCharater(CPacket* clpPacket)
{
    unsigned int deleteId;

    *clpPacket >> deleteId;

    // id 예외처리..

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == deleteId)
        {
            if ((*iter) != nullptr)
                delete* iter;

            iter = g_List.erase(iter);
            break;
        }
    }
}

void PacketProcSCMoveStart(CPacket* clpPacket)
{
    unsigned int dwId;
    BYTE byDir;
    USHORT x;
    USHORT y;

    *clpPacket >> dwId >> byDir >> x >> y;

    // id 예외처리

    if (byDir != LL && byDir == RR)
    {
        //예외 처리
    }

    if (x < dfRANGE_MOVE_LEFT || x > dfRANGE_MOVE_RIGHT)
    {

    }

    if (y < dfRANGE_MOVE_BOTTOM || y > dfRANGE_MOVE_TOP)
    {

    }

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == dwId)
        {
            (*iter)->SetPosition(x, y);
            (*iter)->ActionInput(byDir);
            break;
        }
    }
}

void PacketProcSCMoveStop(CPacket* clpPacket)
{
    unsigned int dwID;
    BYTE byDir;
    USHORT x;
    USHORT y;

    *clpPacket >> dwID >> byDir >> x >> y;

    // id 예외처리..

    if (byDir != LL && byDir == RR)
    {
        //예외 처리
    }

    if (x < dfRANGE_MOVE_LEFT || x > dfRANGE_MOVE_RIGHT)
    {

    }

    if (y < dfRANGE_MOVE_BOTTOM || y > dfRANGE_MOVE_TOP)
    {

    }

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == dwID)
        {
            (*iter)->SetPosition(x, y);
            (*iter)->ActionInput(dfACTION_STAND);
            break;
        }
    }
}

void PacketProcSCAttack1(CPacket* clpPacket)
{
    unsigned int dwID;
    BYTE byDir;
    USHORT x;
    USHORT y;

    *clpPacket >> dwID >> byDir >> x >> y;

    if (byDir != LL && byDir == RR)
    {
        //예외 처리
    }

    if (x < dfRANGE_MOVE_LEFT || x > dfRANGE_MOVE_RIGHT)
    {

    }

    if (y < dfRANGE_MOVE_BOTTOM || y > dfRANGE_MOVE_TOP)
    {

    }

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == dwID)
        {
            (*iter)->SetPosition(x, y);

            if (byDir == LL)
            {
                (*iter)->SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L04, dfDELAY_ATTACK1);
                (*iter)->ActionInput(dfACTION_ATTACK1);
            }
            else
            {
                (*iter)->SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R04, dfDELAY_ATTACK1);
                (*iter)->ActionInput(dfACTION_ATTACK1);
            }
            break;
        }
    }
}

void PacketProcSCAttack2(CPacket* clpPacket)
{
    unsigned int dwID;
    BYTE byDir;
    USHORT x;
    USHORT y;

    *clpPacket >> dwID >> byDir >> x >> y;

    if (byDir != LL && byDir == RR)
    {
        //예외 처리
    }

    if (x < dfRANGE_MOVE_LEFT || x > dfRANGE_MOVE_RIGHT)
    {

    }

    if (y < dfRANGE_MOVE_BOTTOM || y > dfRANGE_MOVE_TOP)
    {

    }

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == dwID)
        {
            (*iter)->SetPosition(x, y);

            if (byDir == LL)
            {
                (*iter)->SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L04, dfDELAY_ATTACK2);
                (*iter)->ActionInput(dfACTION_ATTACK2);
            }
            else
            {
                (*iter)->SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R04, dfDELAY_ATTACK2);
                (*iter)->ActionInput(dfACTION_ATTACK2);
            }
            break;
        }
    }
}

void PacketProcSCAttack3(CPacket* clpPacket)
{
    unsigned int dwID;
    BYTE byDir;
    USHORT x;
    USHORT y;

    *clpPacket >> dwID >> byDir >> x >> y;

    if (byDir != LL && byDir == RR)
    {
        //예외 처리
    }

    if (x < dfRANGE_MOVE_LEFT || x > dfRANGE_MOVE_RIGHT)
    {

    }

    if (y < dfRANGE_MOVE_BOTTOM || y > dfRANGE_MOVE_TOP)
    {

    }

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == dwID)
        {
            (*iter)->SetPosition(x, y);

            if (byDir == LL)
            {
                (*iter)->SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L06, dfDELAY_ATTACK3);
                (*iter)->ActionInput(dfACTION_ATTACK3);
            }
            else
            {
                (*iter)->SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R06, dfDELAY_ATTACK3);
                (*iter)->ActionInput(dfACTION_ATTACK3);
            }

            break;
        }
    }
}

void PacketProcDamage(CPacket* clpPacket)
{
    CBaseObject* pEffect = nullptr;

    unsigned int attackId;
    unsigned int damageId;
    BYTE byHp;

    *clpPacket >> attackId >> damageId >> byHp;

    // attack id 예외처리
    // damage id 예외처리
    if (byHp < 0 || byHp > 100)
    {

    }

    auto iter = g_List.begin();

    for (; iter != g_List.end(); ++iter)
    {
        if ((*iter)->GetObjectID() == attackId)
        {
            USHORT ushX = (*iter)->GetCurX();
            USHORT ushY = (*iter)->GetCurY();
            pEffect = new CEffectObject(1000, eTYPE_EFFECT, ushX, ushY, attackId);
            pEffect->SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_04, dfDELAY_EFFECT);
        }

        if ((*iter)->GetObjectID() == damageId)
        {
            ((CPlayerObject*)(*iter))->SetHP(byHp);
        }
    }

    g_List.push_back(pEffect);
}

void SendEvent()
{

}

void SendPacket()
{
    CPacket clPacket;
    char buffer[500];
    int iUseSize;
    int sendByte;
    DWORD errorCode;
    iUseSize = g_MySession.sendQ.GetUseSize();

    if (iUseSize <= 0)
        return;

    while (TRUE)
    {
        if (iUseSize > clPacket.GetBufferSize())
        {
            g_MySession.sendQ.Peek(clPacket.GetBufferPtr(), clPacket.GetBufferSize());
            sendByte = send(g_MySession.servSock, clPacket.GetBufferPtr(), clPacket.GetBufferSize(), 0);
            
            if (sendByte == SOCKET_ERROR || sendByte == 0)
            {
                errorCode = WSAGetLastError();
                if (errorCode != WSAEWOULDBLOCK)
                {
                    wprintf(L"SendPacket::send Error: %d\n", errorCode);
                    return;
                }
                break;
            }

            if (sendByte < clPacket.GetBufferSize())
                g_MySession.sendQ.MoveFront(sendByte);
            else
                g_MySession.sendQ.MoveFront(clPacket.GetBufferSize());

        }
        else
        {
            g_MySession.sendQ.Peek(clPacket.GetBufferPtr(), iUseSize);
            sendByte = send(g_MySession.servSock, clPacket.GetBufferPtr(), iUseSize, 0);
            if (sendByte == SOCKET_ERROR || sendByte == 0)
            {
                errorCode = WSAGetLastError();
                if (errorCode != WSAEWOULDBLOCK)
                {
                    wprintf(L"SendPacket::send Error: %d\n", errorCode);
                    return;
                }
                break;
            }

            if (sendByte < iUseSize)
                g_MySession.sendQ.MoveFront(sendByte);
            else
                g_MySession.sendQ.MoveFront(iUseSize);

            break;
        }
        /*if (iUseSize > sizeof(buffer))
        {
            g_MySession.sendQ.Peek(buffer, sizeof(buffer));
            sendByte = send(g_MySession.servSock, buffer, sizeof(buffer), 0);
            if (sendByte == SOCKET_ERROR || sendByte == 0)
            {
                errorCode = WSAGetLastError();
                if (errorCode != WSAEWOULDBLOCK)
                {
                    wprintf(L"SendPacket::send Error: %d\n", errorCode);
                    return;
                }
                break;
            }

            if (sendByte < sizeof(buffer))
                g_MySession.sendQ.MoveFront(sendByte);
            else
                g_MySession.sendQ.MoveFront(sizeof(buffer));

        }
        else
        {
            g_MySession.sendQ.Peek(buffer, iUseSize);
            sendByte = send(g_MySession.servSock, buffer, iUseSize, 0);
            if (sendByte == SOCKET_ERROR || sendByte == 0)
            {
                errorCode = WSAGetLastError();
                if (errorCode != WSAEWOULDBLOCK)
                {
                    wprintf(L"SendPacket::send Error: %d\n", errorCode);
                    return;
                }
                break;
            }

            if (sendByte < iUseSize)
                g_MySession.sendQ.MoveFront(sendByte);
            else
                g_MySession.sendQ.MoveFront(iUseSize);

            break;
        }*/
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        //g_hOldIMC = ImmAssociateContext(hWnd, NULL);
        //AllocConsole();
        //FILE* fPtr; // 아무 의미 없음
        //_wfreopen_s(&fPtr, L"CONOUT$", L"wt", stdout);

        InitSocket(hWnd);
    }
    break;

    case WM_NETWORK:
    {
        if (WSAGETSELECTERROR(lParam))
        {
            wprintf(L"WSAGETSELECTERROR!!!\n");
            return 0;
        }

        switch (WSAGETSELECTEVENT(lParam))
        {
        case FD_CONNECT:
            g_MySession.bConnect = TRUE;
            break;
        case FD_READ:
            ReadEvent();
            break;
        case FD_WRITE:
            SendEvent();
            break;
        case FD_CLOSE:
            closesocket(g_MySession.servSock);
            exit(1);
            break;
        }
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_ACTIVATEAPP:
        g_bActiveApp = (BOOL)wParam;
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        ImmAssociateContext(hWnd, g_hOldIMC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}