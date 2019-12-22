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
#define CIRCLE_RADIUS 4
#define BORDER_THICKNESS CIRCLE_RADIUS * 2

class XonixManager
{
private:
	Image *petImage; // Текущая цветная заполненная картинка.
	Image *petImageOutline; // Текущая картинка-контур.
	
	// Верхний левый угол картинки.
	int x0;
	int y0;

	// Размер картинки.
	int width;
	int height;

	// Словарь картинок:
	// ключ - номер уровня,
	// значение - структура, содержащая пару картинок: заполненную и контур.
	map <int, PairImages> imagePathes;
	int level;
	int enemyCount;

	MainCircle mainCircle;
	vector<EnemyCircle> enemyCircles;

	vector<Point> mainCirclePath;
	vector<Rect> capturedField;

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
	bool MoveCircle(HDC);
	void OnPaint(HDC);
	void DrawCircle(HDC, SimpleCircle);
	void AddPointToMainCirclePath();
	void ZoomImageToFitRect(int*, int*, int, int);
};
