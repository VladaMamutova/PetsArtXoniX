﻿#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <string>
#include <mmsystem.h>
#include <stdio.h>
#include "XonixManager.h"

using namespace Gdiplus; // Для отрисовки фоновой картинки.
using namespace std;
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

#define ID_STATUS_BAR 1001
#define ID_START_NEW_GAME 1002
#define ID_EXIT 1003
#define ID_BUTTON 1004
#define ID_SOUNDS_ON 1005
#define ID_SOUNDS_OFF 1006
#define ID_TOOLBAR 1007
#define ID_MOVE_LEFT 1008
#define ID_MOVE_TOP 1009
#define ID_MOVE_RIGHT 1010
#define ID_MOVE_BOTTOM 1011

XonixManager* xonixManager;
HANDLE drawCirclesThread;
bool gameStarted = false;
bool soundsOn;

LONG WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL RegWinClass(WNDPROC, LPCTSTR, HBRUSH);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI DrawCirclesProc(LPVOID);

HINSTANCE hInst;
TCHAR progName[] = "MainWindow";
int bottomMenuHeight = 60;

static HWND statusBar;
static HWND mainMenu;
static HWND button;
static HWND toolBar;
static TBBUTTON toolBarButtons[5];

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	MSG msg;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	
	// Инициализируем GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (!RegWinClass((WNDPROC)WinProc, progName,
		(HBRUSH)GetStockObject(COLOR_WINDOW + 1)))
	{
		return false;
	}

	int width = 450;
	int height = 600;
	
	HWND hwnd = CreateWindow(progName, "Pets ArtXoniX", WS_OVERLAPPED |
		WS_CAPTION | WS_SYSMENU,
		GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2,
		width, height, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Очищаем GDI+.
	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}

LONG WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_CREATE: {
		HMENU popupMenu, mainMenu;
		AppendMenu((popupMenu = CreatePopupMenu()), MF_ENABLED | MFT_STRING,
			(UINT_PTR)ID_START_NEW_GAME, "Начать новую игру");	
		AppendMenu(popupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(popupMenu, MFT_STRING, (UINT_PTR)ID_EXIT, "Выйти");
		AppendMenu((mainMenu = CreateMenu()), MF_ENABLED | MF_POPUP, (UINT_PTR)popupMenu, "Игра");
		SetMenu(hWnd, mainMenu);
		DrawMenuBar(hWnd);

		statusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd, ID_STATUS_BAR);

		RECT rect;
		GetClientRect(hWnd, &rect);

		CreateWindow("static", "Звуки:",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			10, rect.bottom - 50, 50, 20, hWnd, NULL, NULL, NULL);
		HWND soundsOnButton = CreateWindow("button", "Вкл.",
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			70, rect.bottom - 50, 60, 20, hWnd, (HMENU)ID_SOUNDS_ON, hInst, NULL);
		CreateWindow("button", "Выкл.",
			WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
			130, rect.bottom - 50, 60, 20, hWnd, (HMENU)ID_SOUNDS_OFF, hInst, NULL);
		SendMessage(soundsOnButton, BM_SETCHECK, 1, 0);
		soundsOn = true;

		toolBar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE, ID_TOOLBAR,
			0, HINST_COMMCTRL, IDB_STD_LARGE_COLOR, 0, 0, 0, 0, 0, 0, sizeof(TBBUTTON));

		HIMAGELIST imageList;
		if ((imageList = ImageList_Create(16, 16, ILC_COLOR32, 4, 0)) != NULL) {
			HBITMAP bitmap;
			int indexLeft;
			int indexTop;
			int indexRight;
			int indexBottom;

			bitmap = (HBITMAP)LoadImage(NULL, "resources//left-arrow.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			indexLeft = ImageList_Add(imageList, bitmap, (HBITMAP)NULL);
			DeleteObject(bitmap);

			bitmap = (HBITMAP)LoadImage(NULL, "resources//top-arrow.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			indexTop = ImageList_Add(imageList, bitmap, (HBITMAP)NULL);
			DeleteObject(bitmap);

			bitmap = (HBITMAP)LoadImage(NULL, "resources//right-arrow.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			indexRight = ImageList_Add(imageList, bitmap, (HBITMAP)NULL);
			DeleteObject(bitmap);

			bitmap = (HBITMAP)LoadImage(NULL, "resources//bottom-arrow.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			indexBottom = ImageList_Add(imageList, bitmap, (HBITMAP)NULL);
			DeleteObject(bitmap);

			int imageListId = 0;
			SendMessage(toolBar, TB_SETIMAGELIST, imageListId, (LPARAM)imageList);
			
			toolBarButtons[0].fsStyle = TBSTYLE_SEP;

			toolBarButtons[1].iBitmap = MAKELONG(indexLeft, imageListId);
			toolBarButtons[1].idCommand = ID_MOVE_LEFT;
			toolBarButtons[1].fsState = TBSTATE_ENABLED;
			toolBarButtons[2].iBitmap = MAKELONG(indexTop, imageListId);
			toolBarButtons[2].idCommand = ID_MOVE_TOP;
			toolBarButtons[2].fsState = TBSTATE_ENABLED;
			toolBarButtons[3].iBitmap = MAKELONG(indexRight, imageListId);
			toolBarButtons[3].idCommand = ID_MOVE_RIGHT;
			toolBarButtons[3].fsState = TBSTATE_ENABLED;
			toolBarButtons[4].iBitmap = MAKELONG(indexBottom, imageListId);
			toolBarButtons[4].idCommand = ID_MOVE_BOTTOM;
			toolBarButtons[4].fsState = TBSTATE_ENABLED;

			SendMessage(toolBar, TB_ADDBUTTONS, 5, (LPARAM)(&toolBarButtons));
		}

		/*button = CreateWindow("button", "Начать заново", WS_TABSTOP | WS_CHILD | WS_OVERLAPPED|
			WS_VISIBLE | WS_BORDER, rect.right - 170, 4, 160, 22, hWnd, (HMENU)ID_BUTTON, NULL, NULL);
*/
		//edit_window = CreateWindowEx(WS_EX_STATICEDGE, L"edit", NULL,
		//	WS_CHILD | WS_VISIBLE | ES_MULTILINE, 230, 400, 30, 20, hWnd, (HMENU)EDIT_BOX, NULL, NULL);

	/*	statusBar = CreateWindowEx("statusbar", "", NULL, WS_CHILD | WS_VISIBLE,
			0, rect.bottom - 40, rect.right - rect.left, 40, hWnd, NULL, hInst, NULL);*/
		//button = CreateWindow("button", "Одиночный выстрел",
		//	WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, 0, 300, 20, hWnd,
		//	(HMENU)ID_BUTTON, NULL, NULL);

		xonixManager = new XonixManager(Rect(0, 30, rect.right - rect.left,
			rect.bottom - rect.top - bottomMenuHeight - 30));
		xonixManager->StartNewGame();
		drawCirclesThread = CreateThread(NULL, 0, DrawCirclesProc, (LPVOID)hWnd, 0, NULL);
		gameStarted = true;

		TCHAR result[50];
		wsprintf(result, "Захвачено %d%% территории", (int)xonixManager->GetCapturedFieldPersentage());
		SendMessage(statusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)result);

		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_LEFT: {
			xonixManager->SetLeftMove();
			break;
		}
		case VK_RIGHT: {
			xonixManager->SetRightMove();
			break;
		}
		case VK_UP: {
			xonixManager->SetTopMove();
			break;
		}
		case VK_DOWN: {
			xonixManager->SetBottomMove();
			break;
		}
		default:
			break;
		}
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_START_NEW_GAME:
		{
			xonixManager->StartNewGame();
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			TCHAR result[50];
			wsprintf(result, "Захвачено %d%% территории", (int)xonixManager->GetCapturedFieldPersentage());
			SendMessage(statusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)result);
			break;
		}
		case ID_SOUNDS_ON: {
			soundsOn = true;
			break;
		}
		case ID_SOUNDS_OFF: {
			soundsOn = false;
			break;
		}
		case ID_MOVE_LEFT: {
			xonixManager->SetLeftMove();
			break;
		}
		case ID_MOVE_TOP: {
			xonixManager->SetTopMove();
			break;
		}
		case ID_MOVE_RIGHT: {
			xonixManager->SetRightMove();
			break;
		}
		case ID_MOVE_BOTTOM: {
			xonixManager->SetBottomMove();
			break;
		}
		case ID_BUTTON: {
			xonixManager->StartNewGame();
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			TCHAR result[50];
			wsprintf(result, "Захвачено %d%% территории", (int)xonixManager->GetCapturedFieldPersentage());
			SendMessage(statusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)result);
			break;
		}
		}
		break;
	}
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		ps.fErase = false;

		// Получаем размер экрана.
		RECT rect;
		GetClientRect(hWnd, &rect);

		// Двойная буферизация.
		HDC buffHdc = CreateCompatibleDC(hdc);
		HBITMAP buffHbm = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		HANDLE buffHan = SelectObject(buffHdc, buffHbm);

		// Заливаем фон с помощью градиентной кисти.
		Graphics graphics(buffHdc);
		Rect bounds(0, 0, rect.right, rect.bottom - bottomMenuHeight);
		LinearGradientBrush brush(bounds, Color(100, 200, 0), Color(200, 200, 0),
			LinearGradientModeForwardDiagonal);
		graphics.FillRectangle(&brush, bounds);

		// Перерисовываем все объекты в игре.
		xonixManager->OnPaint(buffHdc);

		BitBlt(hdc, 0, 0, rect.right, rect.bottom - bottomMenuHeight, buffHdc, 0, 0, SRCCOPY);

		// Освобождаем память.
		SelectObject(buffHdc, buffHan);
		DeleteObject(buffHbm);
		DeleteObject(buffHdc);

		EndPaint(hWnd, &ps);

		break;
	}
	case WM_GETMINMAXINFO:
	{
		// Устанавливаем минимальный размер окна.
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 200;
		mmi->ptMinTrackSize.y = 250;
		break;
	}
	case WM_DESTROY:
		gameStarted = false;
		if (drawCirclesThread != NULL) {
			PostThreadMessage((DWORD)drawCirclesThread, WM_QUIT, 0, 0);
			CloseHandle(drawCirclesThread);
		}
		delete xonixManager;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

DWORD WINAPI DrawCirclesProc(LPVOID hwnd) {
	HWND hWnd = (HWND)hwnd;
	HDC hdc = GetDC(hWnd);
	while (true) {
		while (gameStarted) {
			if (xonixManager->MoveCircle(hdc)) {
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);

				TCHAR result[50];
				wsprintf(result, "Захвачено %d%% территории", (int)xonixManager->GetCapturedFieldPersentage());
				SendMessage(statusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)result);

				if (xonixManager->IsGameOver()) {
					if (soundsOn) {
						PlaySound("resources//game-over.wav", NULL, SND_FILENAME | SND_ASYNC);
					}
					MessageBox(hWnd, "Вы проиграли :с", "Игра закончена", MB_OK | MB_APPLMODAL);
					xonixManager->StartNewGame();
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
					wsprintf(result, "Захвачено %d%% территории", (int)xonixManager->GetCapturedFieldPersentage());
					SendMessage(statusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)result);
				}

				if (xonixManager->IsAWin())
				{
					if (soundsOn) {
						PlaySound("resources//winner.wav", NULL, SND_FILENAME | SND_ASYNC);
					}
					MessageBox(hWnd, "Победа!!!", "Игра закончена", MB_OK | MB_APPLMODAL);
					xonixManager->StartNewGame();
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
					wsprintf(result, "Захвачено %d%% территории", (int)xonixManager->GetCapturedFieldPersentage());
					SendMessage(statusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)result);
				}
			}
			Sleep(100);
		}
		Sleep(100);
	}
	ReleaseDC((HWND)hWnd, hdc);
	return 0;
}

BOOL RegWinClass(WNDPROC proc, LPCTSTR lpszClassName, HBRUSH backgroundBrush) {
	WNDCLASS w;
	w.lpszClassName = lpszClassName;
	w.hInstance = hInst;
	w.lpfnWndProc = proc;
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	w.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	w.lpszMenuName = NULL;
	w.hbrBackground = backgroundBrush;
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;
	return RegisterClass(&w) != 0;
}
