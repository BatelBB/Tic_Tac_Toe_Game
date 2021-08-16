// Tic_Tac_Toe.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Tic_Tac_Toe.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(LTGRAY_BRUSH); //The color of the background
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APP));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

//Global variables for our game
const int CELL_SIZE = 100;
HBRUSH hbr1, hbr2;
HICON hIcon1, hIcon2;
int playerTurn = 1;
int gameBoard[9] = { 0,0,0,0,0,0,0,0,0 };
int winner = 0;
int wins[3];
//A helper function to create the rectangle of the game
BOOL GetGameBoardRect(HWND hwnd, RECT* pRect) {
    RECT rc;
    if (GetClientRect(hwnd, &rc)) {
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        pRect->left = (width - CELL_SIZE * 3) / 2; //becuse we need 3 cells in a row
        pRect->top = (height - CELL_SIZE * 3) / 2;

        pRect->right = pRect->left + CELL_SIZE * 3;
        pRect->bottom = pRect->top + CELL_SIZE * 3;
        return TRUE;
    }
    SetRectEmpty(pRect);
    return FALSE;
}
//a helper function to create the lines of the game
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}

//a helper function to return the cell number of the board
int GetCellNumberFromPoint(HWND hwnd, int x, int y) {

    POINT pt = { x, y};
    RECT rc;
    if (GetGameBoardRect(hwnd, &rc)) {
        //Gets the cell dimentions.
        if (PtInRect(&rc, pt))
        {
            //User clicked inside the game board
            //Normalize (0 to 3* CELL_SIZE)
            x = pt.x - rc.left; //using the left as a reference point
            y = pt.y - rc.top; //how far is the y from the top edge

            int column = x / CELL_SIZE;
            int row = y / CELL_SIZE;

            //convert to index (0 to 8)
            return column + row * 3;

        }
    }
    return -1; //Outside of the game board or failure
}

BOOL GetCellRect(HWND hWnd, int index, RECT* pRect) {
    RECT rcBoard;
    SetRectEmpty(pRect);
    if (index < 0 || index>8)
        return false;
    if (GetGameBoardRect(hWnd, &rcBoard)) {
        //Convert index from 0 to 8 in x,y pair
        int y = index / 3; //Row number
        int x = index % 3; //Column number

        pRect->left = rcBoard.left + x * CELL_SIZE +1;
        pRect->top = rcBoard.top + y * CELL_SIZE+1;
        pRect->right = pRect->left +CELL_SIZE-1;
        pRect->bottom = pRect->top + CELL_SIZE-1;

        return true;


    }
    return false;
}
/*
Returns:
0 - No winner
1 - Player wins
2 - Player wins
3 - It's a draw

0,1,2
3,4,5
6,7,8
*/
int GetWinner(int wins[3]) {
    int cells[] = { 0,1,2,3,4,5,6,7,8, 0,3,6,1,4,7,2,5,8,0,4,8,2,4,6 };
    //check for winner
    for (int i = 0; i < ARRAYSIZE(cells); i += 3) {
        if (0!=gameBoard[cells[i]] && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]]) {
            //We have a winner
            wins[0] = cells[i];
            wins[1] = cells[i+1];
            wins[2] = cells[i+2];

            return gameBoard[cells[i]];
        }
    }

    //Next, see if we have any cells left empty
    for (int i = 0; i < ARRAYSIZE(gameBoard); i++) {
        if (gameBoard[i] == 0)
            return 0; //continue to play
    }

    return 3; //it's a draw;
}
//A helper function
void ShowTurn(HWND hwnd, HDC hdc) {
    static const WCHAR szTurn1[] = L"Turn: Player 1";
    static const WCHAR szTurn2[] = L"Turn: Player 2";
    const WCHAR* pszTurnText = NULL;

    switch (winner) {
    case 0: //continure to play
        pszTurnText = (playerTurn == 2) ? szTurn2 : szTurn1;
        break;
    case 1: //Player 1 wins
        pszTurnText = L"Player 1 is the winner!";
        break;
    case 2: //Player 2 wins
        pszTurnText = L"Player 2 is the winner!";
        break;
    case 3: //It's a draw
        pszTurnText = L"It's a draw!";
        break;
    }

    RECT rc;
    if (NULL != pszTurnText && GetClientRect(hwnd, &rc)) {
        rc.top = rc.bottom - 48; //To save the length
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
        //DrawIcon(hdc, rc.bottom - 10, 600, playerTurn == 2 ? hIcon2 : hIcon1);

    }
}

void DrawIconCentered(HDC hdc, RECT* pRect, HICON hIcon) {
    const int ICON_WIDTH = GetSystemMetrics(SM_CXICON);
    const int ICON_HEIGHT = GetSystemMetrics(SM_CXICON);

    if (NULL != pRect) {
        int left = pRect->left + ((pRect->right - pRect->left) - ICON_WIDTH) / 2;
        int top = pRect->top + ((pRect->bottom - pRect->top) - ICON_HEIGHT) / 2;
        DrawIcon(hdc, left, top, hIcon);
    }
}

void HighlightWinner(HDC hdc, HWND hWnd) {
    RECT rcWin;
    for (int i = 0; i < 3; i++) {
        if (GetCellRect(hWnd, wins[i], &rcWin)) {
            FillRect(hdc, &rcWin, winner == 2 ? hbr2 : hbr1);
            DrawIconCentered(hdc, &rcWin, winner == 2 ? hIcon2 : hIcon1);
        }
    }
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hbr1 = CreateSolidBrush(RGB(255, 0, 0));
        hbr2 = CreateSolidBrush(RGB(0, 0, 255));

        //Load player icons
        hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_Player1));
        hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_IPlayer2));

    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case ID_FILE_NEWGAME:
            {
                int ret = MessageBox(hWnd, L"Are you sure you want to start a new game?", L"New game", MB_YESNO | MB_ICONQUESTION);
                if (IDYES == ret) {
                    //Reset and start a new game
                    playerTurn = 1;
                    winner = 0;
                    ZeroMemory(gameBoard, sizeof(gameBoard));
                    //Force a paint message
                    InvalidateRect(hWnd, NULL, TRUE); //Post WM_PAINT to our windowProc. It gets queued in our msg queue.
                    UpdateWindow(hWnd); //Forced immediate handling of WM_PAINT

                }
            }
            break;
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
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        //Only handle clicks if it is a player ture (i.e 1 or 2)
        if (playerTurn == 0)
            break;

        int index = GetCellNumberFromPoint(hWnd, xPos, yPos);
        HDC hdc = GetDC(hWnd);
        //A function to write on the screen with every mouse click
        if (NULL != hdc) {
            WCHAR temp[100];
            //wsprintf(temp, L"Index = %d", index);
            //TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

            //Get cell dimension from its index
            if (index != -1) {
                RECT rcCell;
                if (GetCellRect(hWnd, index, &rcCell) && gameBoard[index] == 0) {
                    gameBoard[index] = playerTurn;
                    //FillRect(hdc, &rcCell, playerTurn == 2 ? hbr2 : hbr1);
                    //DrawIcon(hdc, rcCell.left, rcCell.top, playerTurn == 2 ? hIcon2 : hIcon1);
                    DrawIconCentered(hdc, &rcCell, playerTurn == 2 ? hIcon2 : hIcon1);

                    //Check for a winner
                    winner = GetWinner(wins);
                   
                    if (winner == 1 || winner == 2) {
                        HighlightWinner(hdc, hWnd);
                        //We have a winner
                        MessageBox(hWnd, (winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!", L"You Win!", MB_OK | MB_ICONINFORMATION);
                        playerTurn = 0;
                    }
                    else if (winner == 3) {
                        //It's a draw
                        MessageBox(hWnd,L"No one wins this time", L"It's a draw!", MB_OK | MB_ICONINFORMATION);
                        playerTurn = 0;
                    }
                    else if (winner == 0)
                        playerTurn = playerTurn == 1 ? 2 : 1;
                }

                //Display turn
                ShowTurn(hWnd, hdc);

            }
            ReleaseDC(hWnd, hdc);

        }

    }
    break;
        //Case for restricting minimizing the window to a prefered size
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

        pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
        pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;

    }
    break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc;
            if (GetGameBoardRect(hWnd, &rc)) {
                RECT rcClient;
                //Displar player text turn
                if (GetClientRect(hWnd, &rcClient)) {
                    const WCHAR szPlayer1[] = L"Player 1";
                    const WCHAR szPlayer2[] = L"Player 2";

                    SetBkMode(hdc, TRANSPARENT);
                    //Draw Player 1 and Player 2 text
                    SetTextColor(hdc, RGB(255, 255, 0));
                    TextOut(hdc, 16, 16, szPlayer1, ARRAYSIZE(szPlayer1));
                    DrawIcon(hdc, 24,40, hIcon1);
                    SetTextColor(hdc, RGB(0, 0, 255));
                    TextOut(hdc, rcClient.right - 72, 16, szPlayer2, ARRAYSIZE(szPlayer2));
                    DrawIcon(hdc, rcClient.right - 64 ,40, hIcon2);

                    //Display turn
                    ShowTurn(hWnd, hdc);
                }
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                //Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
            }
        
            for (int i = 0; i < 4; i++) {
                //Draw vertical lines
                DrawLine(hdc, rc.left + CELL_SIZE * i , rc.top, rc.left + CELL_SIZE * i, rc.bottom);
                //Draw horizontal lines
                DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);

            }
            //Draw all occupied cells
            RECT rcCell;
            for (int i = 0; i < ARRAYSIZE(gameBoard); i++) {
                if (GetCellRect(hWnd, i, &rcCell) && gameBoard[i] != 0) {
                    //FillRect(hdc, &rcCell, gameBoard[i] == 2 ? hbr2 : hbr1);
                    DrawIconCentered(hdc, &rcCell, gameBoard[i] == 2 ? hIcon2 : hIcon1);
                }

            }
            //Show Winner
            if (winner == 1 || winner == 2)
                HighlightWinner(hdc, hWnd);
            
            EndPaint(hWnd, &ps);
            
        }
        break;
    case WM_DESTROY:
        DeleteObject(hbr1);
        DeleteObject(hbr2);
        DestroyIcon(hIcon1);
        DestroyIcon(hIcon2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
