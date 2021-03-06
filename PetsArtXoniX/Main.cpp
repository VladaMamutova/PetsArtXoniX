﻿#include <windows.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <string>
#include <mmsystem.h>
#include "resource.h"
#include "XonixManager.h"

using namespace Gdiplus; // Для отрисовки фоновой картинки.
using namespace std;
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

#define ID_STATUS_BAR 1001
#define ID_START_NEW_GAME 1002
#define ID_EXIT 1003
#define ID_SETTINGS 1004
#define ID_TOOLBAR 1005
#define ID_MOVE_LEFT 1006
#define ID_MOVE_TOP 1007
#define ID_MOVE_RIGHT 1008
#define ID_MOVE_BOTTOM 1009
#define ID_RESTART 1010

#define RESULT_LINE_LENGTH 100

XonixManager* xonixManager;
HANDLE drawCirclesThread;
bool gameStarted = false;
bool soundsOn;

LONG WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL RegWinClass(WNDPROC, LPCTSTR, HBRUSH);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI DrawCirclesProc(LPVOID);
VOID UpdateStatusBar();

HINSTANCE hInst;
TCHAR progName[] = "MainWindow";

static HWND statusBar;
static HWND mainMenu;
static HWND button;
static HWND toolBar;
static TBBUTTON toolBarButtons[11];
static HMENU popupMenu;

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

	int width = 480;
	int height = 650;

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
		soundsOn = true;
		HMENU mainMenu;
		AppendMenu((popupMenu = CreatePopupMenu()), MF_ENABLED | MFT_STRING,
			(UINT_PTR)ID_START_NEW_GAME, "Начать новую игру");
		AppendMenu(popupMenu, MFT_STRING, (UINT_PTR)ID_SETTINGS, "Настройки");
		AppendMenu(popupMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(popupMenu, MFT_STRING, (UINT_PTR)ID_EXIT, "Выйти");
		AppendMenu((mainMenu = CreateMenu()), MF_ENABLED | MF_POPUP, (UINT_PTR)popupMenu, "Игра");
		SetMenu(hWnd, mainMenu);
		DrawMenuBar(hWnd);

		statusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd, ID_STATUS_BAR);
		// Для разделения статусбара на несколько частей 
		// инициализирем массив ширин его частей и передаём элементу.
		int parts[5];
		parts[0] = 90;
		parts[1] = 150;
		parts[2] = 210;
		parts[3] = 270;
		parts[4] = -1;
		SendMessage(statusBar, SB_SETPARTS, 5, (LPARAM)parts);

		toolBar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT |
			TBSTYLE_TOOLTIPS | CCS_NODIVIDER, ID_TOOLBAR,
			0, HINST_COMMCTRL, IDB_STD_LARGE_COLOR, 0, 0, 0, 0, 0, 0, sizeof(TBBUTTON));

		HIMAGELIST imageList;
		if ((imageList = ImageList_Create(16, 16, ILC_COLOR32, 4, 0)) != NULL) {
			HBITMAP bitmap;
			int indexLeft;
			int indexTop;
			int indexRight;
			int indexBottom;
			int indexRestart;

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

			bitmap = (HBITMAP)LoadImage(NULL, "resources//restart.bmp",
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			indexRestart = ImageList_Add(imageList, bitmap, (HBITMAP)NULL);
			DeleteObject(bitmap);

			int imageListId = 0;
			SendMessage(toolBar, TB_SETIMAGELIST, imageListId, (LPARAM)imageList);

			toolBarButtons[0].fsStyle = TBSTYLE_SEP;

			toolBarButtons[1].iBitmap = MAKELONG(indexLeft, imageListId);
			toolBarButtons[1].idCommand = ID_MOVE_LEFT;
			toolBarButtons[1].fsState = TBSTATE_ENABLED;

			toolBarButtons[2].fsStyle = TBSTYLE_SEP;

			toolBarButtons[3].iBitmap = MAKELONG(indexTop, imageListId);
			toolBarButtons[3].idCommand = ID_MOVE_TOP;
			toolBarButtons[3].fsState = TBSTATE_ENABLED;

			toolBarButtons[4].fsStyle = TBSTYLE_SEP;

			toolBarButtons[5].iBitmap = MAKELONG(indexRight, imageListId);
			toolBarButtons[5].idCommand = ID_MOVE_RIGHT;
			toolBarButtons[5].fsState = TBSTATE_ENABLED;

			toolBarButtons[6].fsStyle = TBSTYLE_SEP;

			toolBarButtons[7].iBitmap = MAKELONG(indexBottom, imageListId);
			toolBarButtons[7].idCommand = ID_MOVE_BOTTOM;
			toolBarButtons[7].fsState = TBSTATE_ENABLED;

			toolBarButtons[8].fsStyle = TBSTYLE_SEP;

			toolBarButtons[9].iBitmap = MAKELONG(indexRestart, imageListId);
			toolBarButtons[9].idCommand = ID_RESTART;
			toolBarButtons[9].fsState = TBSTATE_ENABLED;

			toolBarButtons[10].fsStyle = TBSTYLE_SEP;

			SendMessage(toolBar, TB_ADDBUTTONS, 11, (LPARAM)(&toolBarButtons));
		}

		RECT rect;
		GetClientRect(hWnd, &rect);

		xonixManager = new XonixManager(Rect(0, 40, rect.right - rect.left,
			rect.bottom - rect.top - 70));
		xonixManager->StartNewGame();
		UpdateStatusBar();
		drawCirclesThread = CreateThread(NULL, 0, DrawCirclesProc, (LPVOID)hWnd, 0, NULL);
		gameStarted = true;

		break;
	}
	case WM_NOTIFY: {
		LPTOOLTIPTEXT toolTip;
		toolTip = (LPTOOLTIPTEXT)lParam;
		if (toolTip->hdr.code != TTN_NEEDTEXT) break;

		switch (toolTip->hdr.idFrom) {
		case ID_MOVE_LEFT: {
			toolTip->lpszText = "Направить шарик влево (стрелка \"Влево\")";
			break;
		}
		case ID_MOVE_TOP: {
			toolTip->lpszText = "Направить шарик вверх (стрелка \"Вверх\")";;
			break;
		}
		case ID_MOVE_RIGHT: {
			toolTip->lpszText = "Направить шарик вправо (стрелка \"Вправо\")";
			break;
		}
		case ID_MOVE_BOTTOM: {
			toolTip->lpszText = "Направить шарик вниз (стрелка \"Вниз\")";
			break;
		}
		case ID_RESTART: {
			toolTip->lpszText = "Перезапустить игру";
			break;
		}
		}
		break;
	}
	case WM_CONTEXTMENU: {
		TrackPopupMenu(popupMenu, TPM_RIGHTBUTTON | TPM_TOPALIGN |
			TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), 0, hWnd, NULL);
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
		}
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case ID_START_NEW_GAME:
		case ID_RESTART: {
			xonixManager->StartNewGame();
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			UpdateStatusBar();
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
		case ID_SETTINGS: {
			if (DialogBox(NULL, (LPCTSTR)IDD_DIALOGBAR, hWnd,
				(DLGPROC)DialogProc) == IDOK) {
				xonixManager->StartNewGame();
				UpdateWindow(hWnd);
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateStatusBar();
			}
			break;
		}
		case ID_EXIT: {
			DestroyWindow(hWnd);
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
			Sleep(100);
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
	int round = 0;
	while (true) {
		while (gameStarted) {
			round = xonixManager->GetRound();
			if (xonixManager->MoveCircles(hdc)) {
				if (xonixManager->IsGameOver()) {
					if (soundsOn) {
						PlaySound("resources//game-over.wav", NULL, SND_FILENAME | SND_ASYNC);
					}
					MessageBox(hWnd, "Вы проиграли :с", "Окончание раунда", MB_OK | MB_APPLMODAL);
					xonixManager->StartNewGame();
				}
				else if (xonixManager->IsAWin()) {
					if (soundsOn) {
						PlaySound("resources//winner.wav", NULL, SND_FILENAME | SND_ASYNC);
					}
					InvalidateRect(hWnd, NULL, TRUE);
					UpdateWindow(hWnd);
					UpdateStatusBar();
					MessageBox(hWnd, "Раунд пройден!!!", "Окончание игры", MB_OK | MB_APPLMODAL);
					xonixManager->StartNewRound();
				}
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
				UpdateStatusBar();
			}
			Sleep(xonixManager->GetTimeDelay());
		}
		Sleep(100);
	}
	ReleaseDC((HWND)hWnd, hdc);
	return 0;
}

BOOL CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam,
	LPARAM lParam)
{
	static HWND radioButton;
	static HWND scrollBar;
	static HWND list;
	static HWND comboBox;
	
	// Обязательно static, чтобы при обрабоке новых
	// сообщений окна значения сохранялись.
	static int enemyCount = xonixManager->GetEnemyCount();
	static int speedIndex = xonixManager->GetSpeed();
	static int lives = xonixManager->GetLives();

	switch (msg)
	{
	case WM_INITDIALOG:
	{
		// Устанавливаем переключатель звука.
		radioButton = soundsOn
			? GetDlgItem(hDlg, IDC_RADIO_ON)
			: GetDlgItem(hDlg, IDC_RADIO_OFF);
		radioButton = soundsOn
			? GetDlgItem(hDlg, IDC_RADIO_ON)
			: GetDlgItem(hDlg, IDC_RADIO_OFF);
		SendMessage(radioButton, BM_SETCHECK, 1, 0);
		if (IsDlgButtonChecked(hDlg, IDC_RADIO_ON)) {
			SendMessage(radioButton, BM_SETCHECK, 1, 0);
		}

		// Выводим количество врагов.
		TCHAR enemyCountString[10];
		sprintf_s(enemyCountString, "%d %s", enemyCount, "враг");
		if (enemyCount % 10 > 1 && enemyCount % 10 < 5) {
			lstrcat(enemyCountString, "а");
		}
		else if (enemyCount % 10 == 0 || enemyCount % 10 >= 5) {
			lstrcat(enemyCountString, "ов");
		}

		SetDlgItemText(hDlg, IDC_STATIC_RESULT_ENEMY_COUNT, enemyCountString);

		// Устанавливаем состояние скроллбара.
		scrollBar = GetDlgItem(hDlg, IDC_SCROLLBAR_ENEMY_COUNT);
		SetScrollRange(scrollBar, SB_CTL, 1, 6, TRUE);
		SetScrollPos(scrollBar, SB_CTL, enemyCount, TRUE);

		// Загружаем список возможных скоростей и текущую скорость.
		comboBox = GetDlgItem(hDlg, IDC_COMBO_ENEMY_SPEED);
		SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)ToString(SPEED::LOW));
		SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)ToString(SPEED::AVERAGE));
		SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)ToString(SPEED::HIGH));
		SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)ToString(SPEED::VERY_HIGH));
		SendMessage(comboBox, CB_SETCURSEL, speedIndex, 0);
		SendMessage(comboBox, CB_SETEXTENDEDUI, 1, 0);

		// Устанавливаем значение количества жизней в игре.
		switch (lives)
		{
		case 1: {
			CheckDlgButton(hDlg, IDC_CHECK_1_LIFE, BST_CHECKED);
			break;
		}
		case 3: {
			CheckDlgButton(hDlg, IDC_CHECK_3_LIVES, BST_CHECKED);
			break;
		}
		case 5: {
			CheckDlgButton(hDlg, IDC_CHECK_5_LIVES, BST_CHECKED);
			break;
		}
		default: {
			CheckDlgButton(hDlg, IDC_CHECK_3_LIVES, BST_CHECKED);
			break;
		}
		}

		// Загружаем список результатов.
		list = GetDlgItem(hDlg, IDC_LISTBOX_RESULTS);
		HANDLE file = CreateFileA(  // функция создания ANSI
			"result.txt", GENERIC_READ,	0, NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);

		TCHAR buffer[1];
		DWORD byteRead;
		TCHAR line[RESULT_LINE_LENGTH];
		if (file != INVALID_HANDLE_VALUE)
		{
			do
			{
				int i = 0;
				do {
					ReadFile(file, &buffer, 1, &byteRead, 0);
					if (byteRead != 0 && buffer[0] != '\r' && buffer[0] != '\n') {
						line[i] = buffer[0];
						i++;
					}
				} while (byteRead != 0 && buffer[0] != '\r' && buffer[0] != '\n');
				if (byteRead) {
					line[i] = '\0';
				}
				if (i > 0) {
					SendMessage(list, LB_ADDSTRING, 0, (LPARAM)line);
				}

			} while (byteRead != 0);
		}
		CloseHandle(file);
		return FALSE;
	}
	case WM_HSCROLL: {
		switch (LOWORD(wParam)) {
		case SB_LEFT: {
			enemyCount = 1;
			break;
		}
		case SB_LINELEFT:
		case SB_PAGELEFT:
		{
			enemyCount--;
			break;
		}
		case SB_RIGHT:
		{
			enemyCount = 6;
			break;
		}
		case SB_LINERIGHT:
		case SB_PAGERIGHT: {
			enemyCount++;
			break;
		}
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK: {
			enemyCount = HIWORD(wParam);
			break;
		}
		}
		if (enemyCount >= 6) {
			enemyCount = 6;
			EnableScrollBar(statusBar, SB_CTL, ESB_DISABLE_RIGHT);
		}
		if (enemyCount <= 1) {
			enemyCount = 1;
			EnableScrollBar(statusBar, SB_CTL, ESB_DISABLE_LEFT);
		}
		TCHAR enemyCountString[10];
		sprintf_s(enemyCountString, "%d %s", enemyCount, "враг");
		if (enemyCount % 10 > 1 && enemyCount % 10 < 5) {
			lstrcat(enemyCountString, "а");
		}
		else if (enemyCount % 10 == 0 || enemyCount % 10 >= 5) {
			lstrcat(enemyCountString, "ов");
		}

		SetScrollPos(scrollBar, SB_CTL, enemyCount, TRUE);
		SetDlgItemText(hDlg, IDC_STATIC_RESULT_ENEMY_COUNT, enemyCountString);

		return FALSE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_RADIO_ON: {
			soundsOn = true;
			return FALSE;
		}
		case IDC_RADIO_OFF: {
			soundsOn = false;
			return FALSE;
		}
		case IDC_COMBO_ENEMY_SPEED: {
			if (HIWORD(wParam) == CBN_SELCHANGE) { // CBN_SELCHANGE - изменился номер выбранной строки
				int selectedIndex = (int)SendMessage(comboBox, CB_GETCURSEL, 0, 0L);
				if (selectedIndex != CB_ERR) // если индекс != -1
				{
					speedIndex = selectedIndex;
				}
			}
			return FALSE;
		}
		case IDC_CHECK_1_LIFE: {
			CheckDlgButton(hDlg, IDC_CHECK_1_LIFE, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_3_LIVES, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_5_LIVES, BST_UNCHECKED);
			lives = 1;
			return FALSE;
		}
		case IDC_CHECK_3_LIVES: {
			CheckDlgButton(hDlg, IDC_CHECK_1_LIFE, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_3_LIVES, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_5_LIVES, BST_UNCHECKED);
			lives = 3;
			return FALSE;
		}
		case IDC_CHECK_5_LIVES: {
			CheckDlgButton(hDlg, IDC_CHECK_1_LIFE, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_3_LIVES, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK_5_LIVES, BST_CHECKED);
			lives = 5;
			return FALSE;
		}
		case IDC_BUTTON_SAVE_RESULT: {
			static TCHAR playerName[50];
			GetDlgItemText(hDlg, IDC_EDBOX_NAME, playerName, sizeof(playerName));

			if (strlen(playerName) <= 0) {
				MessageBox(NULL, "Введите ваше имя, чтобы сохранить результаты "
					"последней игры.", "Сохранение результата игры", MB_OK);
				return FALSE;
			}

			TCHAR result[RESULT_LINE_LENGTH];
			sprintf_s(result, "%s: раунд %d, жизней %d, врагов %d, %s скорость\n",
				playerName, xonixManager->GetRound(), xonixManager->GetLives(),
				xonixManager->GetEnemyCount(), ToString((SPEED)xonixManager->GetSpeed()));

			HANDLE file = CreateFile("result.txt", GENERIC_WRITE, 0, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file != INVALID_HANDLE_VALUE)
			{
				SetFilePointer(file, 0, NULL, FILE_END); // Добавляем данные в конец.
				WriteFile(file, result, strlen(result), NULL, NULL);
				CloseHandle(file);
			}
			SendMessage(list, LB_ADDSTRING, 0, (LPARAM)result);
			return FALSE;
		}
		case IDOK: {
			if (xonixManager->GetRound() == 1 && xonixManager->GetCapturedFieldPersentage() == 0 ||
				MessageBox(hDlg, "Ваша игра не закончена. При сохранении настроек текущий "
				"результат игры будет потерян. Продолжить?", "Настройки",
				MB_YESNO | MB_APPLMODAL) == IDYES) {
				xonixManager->SetEnemyCount(enemyCount);
				xonixManager->SetSpeed((SPEED)speedIndex);
				xonixManager->SetLives(lives);
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
			return FALSE;
		}
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		default: return FALSE;
		}
	}
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return FALSE;
	}
	}
	return FALSE; // false - для вызова функции прорисовки окна.
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
VOID UpdateStatusBar() {
	TCHAR result[50];
	wsprintf(result, "Захвачено %d%%", (int)xonixManager->GetCapturedFieldPersentage());
	SendMessage(statusBar, SB_SETTEXT, (WPARAM)0 | SBT_NOBORDERS, (LPARAM)result);
	wsprintf(result, "Раунд: %d", xonixManager->GetRound());
	SendMessage(statusBar, SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)result);
	wsprintf(result, "Жизни: %d", xonixManager->GetLifeCount());
	SendMessage(statusBar, SB_SETTEXT, 2 | SBT_NOBORDERS, (LPARAM)result);
	wsprintf(result, "Врагов: %d", xonixManager->GetEnemyCount());
	SendMessage(statusBar, SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)result);
	wsprintf(result, "Скорость: %s", ToString((SPEED)xonixManager->GetSpeed()));
	SendMessage(statusBar, SB_SETTEXT, 4 | SBT_NOBORDERS, (LPARAM)result);
}
