﻿#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>
#include <map>
#include <vector>
#include <string>
#include "MainCircle.h"
#include "EnemyCircle.h"
#include "Speed.h"

using namespace std;
using namespace Gdiplus;

#define PATH_LENGTH 500

#define IMAGE_COUNT 10
#define CIRCLE_RADIUS 4
#define CELL_SIZE 10
#define BORDER_THICKNESS CELL_SIZE

class XonixManager
{
public:
private:
	enum FieldCellState {
		EMPTY = 0,
		CAPTURED = 1,
		TRAIL = 2,
		ENEMY = 3
	};

	int fieldWidth;
	int fieldHeight;
	int** fieldCells;
	
	// Словарь картинок: ключ - номер раунда, значение - путь к файлу картинки.
	map <int, wchar_t[PATH_LENGTH]> imagePathes;

	Image *petImage; // Текущая картинка.
	int imageWidth;
	int imageHeight;

	// Верхний левый угол картинки.
	int x0;
	int y0;
	
	int round;
	int enemyCount;
	SPEED speed;
	float capturedFieldPercentage;
	int capturedFieldPercentageToWin = 80;
	int lifeCount;
	int lives;

	bool isGameOver;
	bool isAWin;

	MainCircle mainCircle;
	vector<EnemyCircle> enemyCircles;

	void DrawCircle(HDC, SimpleCircle, Point);
	void CheckCell(int x, int y);
	void UpdateField();
	void ZoomImageToFitRect(int*, int*, int, int);
public:
	XonixManager(Gdiplus::Rect);
	~XonixManager();

	// Get-методы.
	bool IsGameOver();
	bool IsAWin();
	int GetEnemyCount();
	int GetRound();
	int GetLives();
	int GetLifeCount();
	int GetSpeed();
	int GetTimeDelay();
	float GetCapturedFieldPersentage();

	// Set-методы.
	void SetTopMove();
	void SetBottomMove();
	void SetLeftMove();
	void SetRightMove();
	void SetEnemyCount(int);
	void SetLives(int);
	void SetSpeed(SPEED);

	// Публичные методы, реализующие логику игры.
	void StartNewGame();
	void StartNewRound();
	void RestartRound();
	bool MoveCircles(HDC);	
	void OnPaint(HDC);
};
