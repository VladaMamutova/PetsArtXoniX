#include <windows.h>
#include <gdiplus.h>
#include "XonixManager.h"

using namespace Gdiplus; // Для отрисовки фоновой картинки.
using namespace std;
#pragma comment (lib,"Gdiplus.lib")

XonixManager* xonixManager;
HANDLE drawCirclesThread;
bool gameStarted = false;

LONG WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL RegWinClass(WNDPROC, LPCTSTR, HBRUSH);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI DrawCirclesProc(LPVOID);

HINSTANCE hInst;
TCHAR progName[] = "MainWindow";

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
	int height = 580;
	
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
		RECT rect;
		GetClientRect(hWnd, &rect);
		xonixManager = new XonixManager(rect.right - rect.left, rect.bottom - rect.top);
		xonixManager->StartNewGame();
		drawCirclesThread = CreateThread(NULL, 0, DrawCirclesProc, (LPVOID)hWnd, 0, NULL);
		gameStarted = true;
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
		Rect bounds(0, 0, rect.right, rect.bottom);
		LinearGradientBrush brush(bounds, Color(100, 200, 0), Color(200, 200, 0),
			LinearGradientModeForwardDiagonal);
		graphics.FillRectangle(&brush, bounds);

		// Перерисовываем все объекты в игре.
		xonixManager->OnPaint(buffHdc);

		BitBlt(hdc, 0, 0, rect.right, rect.bottom, buffHdc, 0, 0, SRCCOPY);

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
				if (xonixManager->IsGameOver()) {
					MessageBox(hWnd, "Вы проиграли :с", "Игра закончена", MB_OK | MB_APPLMODAL);
					xonixManager->StartNewGame();
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
				}

				if (xonixManager->IsAWin())
				{
					MessageBox(hWnd, "Победа!!!", "Игра закончена", MB_OK | MB_APPLMODAL);
					xonixManager->StartNewGame();
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
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
