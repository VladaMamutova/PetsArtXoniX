#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>
#include <map>
#include <vector>
#include "PairImages.h"
#include "MainCircle.h"
#include "EnemyCircle.h"

using namespace std;
using namespace Gdiplus;

#define LEVEL_COUNT 1
#define FIELD_MARGIN 30
#define CELL_SIZE 4

class XonixManager
{
private:
	enum FieldCellState {
		EMPTY,
		MARKED,
		FILLED	
	};

	int x0;
	int y0;
	int fieldWidth;
	int fieldHeight;
	int** fieldCells;

	Image *petImage; // Текущая цветная заполненная картинка.
	Image *petImageOutline; // Текущая картинка-контур.

	// Словарь картинок:
	// ключ - номер уровня,
	// значение - структура, содержащая пару картинок: заполненную и контур.
	map <int, PairImages> imagePathes;
	int level;
	int enemyCount;

	MainCircle mainCircle;
	vector<EnemyCircle> enemyCircles;

	void LoadPetImage();
	void InitMainCircle(int x, int y);
	void InitEnemyCircles(Rect bounds);
public:
	XonixManager();
	~XonixManager();
	void StartNewGame(int, int);
	void SetTopMove();
	void SetBottomMove();
	void SetLeftMove();
	void SetRightMove();
	void MoveCircle(HDC);
	void OnPaint(HDC, RECT);
	void DrawCircle(HDC, SimpleCircle);
	void ZoomImageToFitRect(int*, int*, int, int);
};
