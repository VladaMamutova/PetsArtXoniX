#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>
#include <map>
#include <vector>
#include <string>
#include "PairImages.h"
#include "MainCircle.h"
#include "EnemyCircle.h"

using namespace std;
using namespace Gdiplus;

#define LEVEL_COUNT 1
#define CIRCLE_RADIUS 4
#define CELL_SIZE 10
#define BORDER_THICKNESS CELL_SIZE

class XonixManager
{
private:
	enum FieldCellState {
		EMPTY,
		MARKED,
		FILLED
	};

	int fieldWidth;
	int fieldHeight;
	int** fieldCells;

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
	int capturedFieldPercentageToWin = 80;
	int enemyCount;
	float capturedFieldPercentage;

	bool isGameOver;
	bool isAWin;

	MainCircle mainCircle;
	vector<EnemyCircle> enemyCircles;

	vector<Point> mainCirclePath;
	vector<Rect> capturedField;

	void LoadPetImage();
	void InitMainCircle(int x, int y);
	void InitEnemyCircles(Rect bounds);
public:
	XonixManager(Gdiplus::Rect);
	~XonixManager();
	bool IsGameOver();
	bool IsAWin();
	bool GetEnemyCount();
	void StartNewGame();
	void SetTopMove();
	void SetBottomMove();
	void SetLeftMove();
	void SetRightMove();
	void SetEnemyCount(int);
	float GetCapturedFieldPersentage();
	bool MoveCircle(HDC);
	void OnPaint(HDC);
	void DrawCircle(HDC, SimpleCircle, Point);
	void AddPointToMainCirclePath();
	void CheckCell(int x, int y);
	void UpdateField();
	void ZoomImageToFitRect(int*, int*, int, int);
};
