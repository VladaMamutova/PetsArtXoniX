#include <tchar.h>
#include <windows.h>
#include <gdiplus.h>
#include <cstring>
#include <string>
#include <iostream>
#include "XonixManager.h"

using namespace Gdiplus; // Для отрисовки фоновой картинки.
using namespace std;
#pragma comment (lib,"Gdiplus.lib")

XonixManager* xonixManager;

LONG WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL RegWinClass(WNDPROC, LPCTSTR, HBRUSH);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

	xonixManager = new XonixManager();
	xonixManager->StartNewGame();

	int width = 450;
	int height = 580;
	HWND hwnd = CreateWindow(progName, "Pets ArtXoniX", WS_OVERLAPPEDWINDOW,
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

		break;
	}
	case WM_COMMAND:

		break;
	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);
		
		// Получаем размер экрана.
		RECT rect;
		GetClientRect(hWnd, &rect);

		// Заливаем фон с помощью градиентной кисти.
		Graphics graphics(hdc);
		Rect bounds(0, 0, rect.right, rect.bottom);
		LinearGradientBrush brush(bounds, Color(100, 200, 0), Color(200, 200, 0),
			LinearGradientModeForwardDiagonal);
		graphics.FillRectangle(&brush, bounds);
		
		// Перерисовываем все объекты в игре.
		xonixManager->OnPaint(hdc, rect);

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
		delete xonixManager;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

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
