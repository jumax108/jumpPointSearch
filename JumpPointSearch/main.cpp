// JumpPointSearch.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Resource.h"
#include "myLinkedList.h"
#include "JumpPointSearch.h"

#define MAX_LOADSTRING 100

CJumpPointSearch* jps;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUMPPOINTSEARCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JUMPPOINTSEARCH));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUMPPOINTSEARCH));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(43, 43, 43));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_JUMPPOINTSEARCH);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

int mapWidth;
int mapHeight;

int mouseX;
int mouseY;
int blockSize;
int timerDelay;

bool mouseDown;
CJumpPointSearch::MAP_STATE changeThisState;

CJumpPointSearch::iterator* endNodeIter;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:

        mapWidth = 50;
        mapHeight = 25;
        blockSize = 25;
        timerDelay = 200;
        mouseDown = false;

        jps = new CJumpPointSearch(mapWidth, mapHeight);
        new (&jps->_end) CJumpPointSearch::stCoord(-1, -1);
        new (&jps->_start) CJumpPointSearch::stCoord(-1, -1);
        break;
    case WM_TIMER:
    {
        void* result = jps->pathFindSingleLoop();
        if ((int)result != 1) {
            KillTimer(hWnd, 1);
            if (result != nullptr) {
                // 찾음
                endNodeIter = new CJumpPointSearch::iterator(jps->makePath((CJumpPointSearch::stNode*)result));
            }
        }
        InvalidateRect(hWnd, nullptr, true);
    }
        break;
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
    case WM_MOUSEMOVE:
        {
            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
            do {
                if (mouseDown == false) {
                    break;
                }
                int blockY = mouseY / blockSize;
                int blockX = mouseX / blockSize;
                *(jps->map(blockY, blockX)) = changeThisState;
                RECT rt;
                rt.left = blockX * blockSize;
                rt.right = rt.left + blockSize;
                rt.top = blockY * blockSize;
                rt.bottom = rt.top + blockSize;
                InvalidateRect(hWnd, &rt, true);
            } while (false);
        }
        break;

    case WM_MBUTTONDOWN:
        break;
    case WM_RBUTTONDOWN:
        delete(endNodeIter);
        endNodeIter = nullptr;
        jps->pathFindInit();
        SetTimer(hWnd, 1, timerDelay, nullptr);
        break;
    case WM_LBUTTONDOWN: {
        mouseDown = true;
        int blockY = mouseY / blockSize;
        int blockX = mouseX / blockSize;
        switch (*(jps->map(blockY, blockX))) {
            case CJumpPointSearch::MAP_STATE::ROAD:
                changeThisState = CJumpPointSearch::MAP_STATE::WALL;
                break;
            case CJumpPointSearch::MAP_STATE::WALL:
                changeThisState = CJumpPointSearch::MAP_STATE::ROAD;
                break;
        }
        break;
    }
    case WM_LBUTTONUP:
        mouseDown = false;
        break;
    case WM_CHAR:
    {
        switch (wParam) {
            case 's':
            case 'e':
            {
                int blockY = mouseY / blockSize;
                int blockX = mouseX / blockSize;
                if (blockY < 0 || blockX < 0 || blockY >= jps->height() || blockX >= jps->width()) {
                    break;
                }

                CJumpPointSearch::stCoord* target;
                if (wParam == 's') {
                    target = &jps->_start;
                }
                else {
                    target = &jps->_end;
                }

                new (target) CJumpPointSearch::stCoord(blockY, blockX);
                InvalidateRect(hWnd, nullptr, true);
                break;
            }
            case 'c':
            {

                delete(endNodeIter);
                endNodeIter = nullptr;
                KillTimer(hWnd, 1);
                jps->pathFindInit();
                InvalidateRect(hWnd, nullptr, true);
                break;
            }
            case 'z':
            {
                delete(endNodeIter);
                endNodeIter = nullptr;
                KillTimer(hWnd, 1);
                delete(jps);
                jps = new CJumpPointSearch(mapWidth, mapHeight);
                InvalidateRect(hWnd, nullptr, true);
                break;
            }
            case 't':
            {
                for (int testCnt = 0; testCnt < 30; ++testCnt) {
                    WCHAR fileName[50];
                    swprintf_s(fileName, 50, L".\\testImage\\%04d.bmp", testCnt);
                    CJumpPointSearch::test(fileName);
                }
                MessageBoxW(hWnd, L"테스트 끝 !", L"JumpPointSearch", MB_OK);
            }
                break;
            case 'p':
            {
                jps->lineTo(jps->_start._x, jps->_start._y, jps->_end._x, jps->_end._y, true);

                HDC hdc = GetDC(hWnd);
                HBRUSH hBrush = CreateSolidBrush(RGB(123, 0, 123));
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

                MoveToEx(hdc, jps->_start._x * blockSize + (blockSize / 2), jps->_start._y* blockSize + (blockSize / 2), nullptr);
                LineTo(hdc, jps->_end._x* blockSize + (blockSize / 2), jps->_end._y* blockSize + (blockSize / 2));

                CJumpPointSearch::iterator iter = jps->lineBegin();
                for (; iter != jps->lineEnd(); ++iter) {

                    //LineTo(hdc,  + (blockSize / 2), (*iter)->_coord->_y * blockSize + (blockSize / 2));
                    int left = (*iter)->_coord->_x * blockSize + 1;
                    int top = (*iter)->_coord->_y * blockSize + 1;
                    int right = left + blockSize - 1;
                    int bottom = top + blockSize - 1;
                 //   Rectangle(hdc, left, top, right, bottom);
                }

                SelectObject(hdc, hOldBrush);
                DeleteObject(hBrush);

                InvalidateRect(hWnd, nullptr, true);

            }
            case 'l': {
                jps->nodeSkip();
                InvalidateRect(hWnd, nullptr, false);
            }
                break;
            break;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        if (endNodeIter == nullptr) {
            jps->print(hdc, blockSize, jps->pathEnd());
        }
        else {
            jps->print(hdc, blockSize, *endNodeIter);
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_MOUSEWHEEL: {
        short wheelMove = HIWORD(wParam);
        if (wheelMove > 0) {
            blockSize += 1;
        }
        else {
            if (blockSize - 1 > 0) {
                blockSize -= 1;
            }
        }
        InvalidateRect(hWnd, nullptr, true);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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
