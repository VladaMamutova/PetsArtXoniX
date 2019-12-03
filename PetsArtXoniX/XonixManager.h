#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>
#include <map>
#include "PairImages.h"
#include "MainCircle.h"
#include "EnemyCircle.h"

using namespace std;
using namespace Gdiplus;

#define LEVEL_COUNT 1

class XonixManager
{
private:
	Image *petImage; // Текущая цветная заполненная картинка.
	Image *petImageOutline; // Текущая картинка-контур.

	// Словарь картинок:
	// ключ - номер уровня,
	// значение - структура, содержащая пару картинок: заполненную и контур.
	map <int, PairImages> imagePathes;
	int level;

	MainCircle mainCircle;
	//vector<EnemyCircle> circles;

	void LoadPetImage();
	void InitMainCircle(int x, int y);
	//void InitEnemyCircles();
public:
	XonixManager();
	~XonixManager();
	void StartNewGame(Rect rect);
	void SetTopMove();
	void SetBottomMove();
	void SetLeftMove();
	void SetRightMove();
	void MoveCircle(HDC, RECT);
	void OnPaint(HDC, RECT);
};
