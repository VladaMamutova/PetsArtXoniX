#include "XonixManager.h"
#include <math.h>
#include <time.h>
#include <stack>

using namespace Gdiplus;

#pragma comment(lib, "GdiPlus.lib")

XonixManager::XonixManager(Rect gameRect)
{
	speed = SPEED::AVERAGE;
	enemyCount = 1;
	round = 1;
	lives = 3;
	isGameOver = false;
	isAWin = false;
	capturedFieldPercentage = 0;

	// Получаем путь к текущей директории.
	wchar_t path[PATH_LENGTH];
	GetModuleFileNameW(NULL, path, 500); // Путь к .exe.
	bool isCropped = false;
	for (size_t i = sizeof(path) - 1; i > 0 && !isCropped; i--)
	{
		if (path[i] == '\\') // На последнем слэше обрезаем путь.
		{
			path[i] = '\0';
			isCropped = true;
		}
	}

	// Заполняем словарь картинок.
	wchar_t name[IMAGE_COUNT][50] = { L"yorkshire-terrier.jpg",
		L"siamese-cat.jpg", L"spitz-in-tulips.jpg", L"cat-in-the-pant-lag.jpg",
		 L"little-labrador.jpg", L"fluffy-cat.jpg", L"dog-behind-the-wheel.jpg",
		 L"cat-with-glasses.jpg", L"dog-in-bubles.jpg", L"cat-in-the-bed.jpg" };
	for (int i = 0; i < IMAGE_COUNT; i++) {
		swprintf_s(imagePathes[i + 1], L"%ls\\img\\%ls", path, name[i]);
	}

	petImage = new Image(imagePathes[round]);

	// По размеру окна вычисляем размер картинки.
	imageWidth = petImage->GetWidth();
	imageHeight = petImage->GetHeight();
	ZoomImageToFitRect(&imageWidth, &imageHeight,
		gameRect.Width, gameRect.Height);

	// Корректируем размеры с учётом размера ячейки поля и рамки.
	imageWidth -= imageWidth % CELL_SIZE + 2 * BORDER_THICKNESS;
	imageHeight -= imageHeight % CELL_SIZE + 2 * BORDER_THICKNESS;
	
	// Находим верхний левый отступ от краёв окна так,
	// чтобы картинка размещалась посередине.
	x0 = (gameRect.Width - imageWidth) / 2 + gameRect.GetLeft();
	y0 = (gameRect.Height - imageHeight) / 2 + gameRect.GetTop();

	// Вычисляем размер поля по количеству ячеек и создаём его.
	fieldWidth = imageWidth / CELL_SIZE;
	fieldHeight = imageHeight / CELL_SIZE;
	fieldCells = new int*[fieldHeight];
	for (int i = 0; i < fieldHeight; ++i)
		fieldCells[i] = new int[fieldWidth];
}

XonixManager::~XonixManager()
{
	for (int i = 0; i < fieldHeight; ++i)
		delete[] fieldCells[i];
	delete[] fieldCells;

	if (petImage) {
		delete petImage;
	}
}

bool XonixManager::IsGameOver()
{
	return isGameOver;
}

bool XonixManager::IsAWin()
{
	return isAWin;
}

int XonixManager::GetEnemyCount()
{
	return enemyCount;
}

int XonixManager::GetRound()
{
	return round;
}

int XonixManager::GetLives()
{
	return lives;
}

int XonixManager::GetLifeCount()
{
	return lifeCount;
}

int XonixManager::GetSpeed()
{
	return speed;
}

int XonixManager::GetTimeDelay() {
	switch (speed)
	{
	case LOW: return 130;
	case AVERAGE: return 100;
	case HIGH: return 75;
	case VERY_HIGH: return 50;
	default: return 100;
	}
}

void XonixManager::SetTopMove() {
	mainCircle.SetDirection(Direction::Up);
}

void XonixManager::SetBottomMove() {
	mainCircle.SetDirection(Direction::Down);
}

void XonixManager::SetLeftMove() {
	mainCircle.SetDirection(Direction::Left);
}

void XonixManager::SetRightMove() {
	mainCircle.SetDirection(Direction::Right);
}

void XonixManager::SetEnemyCount(int enemyCount)
{
	this->enemyCount = enemyCount;
}

void XonixManager::SetLives(int lives)
{
	this->lives = lives;
}

void XonixManager::SetSpeed(SPEED speed)
{
	this->speed = speed;
}

float XonixManager::GetCapturedFieldPersentage()
{
	return capturedFieldPercentage;
}

void XonixManager::StartNewGame() {
	lifeCount = lives;
	round = 0;
	StartNewRound();
}

void XonixManager::StartNewRound() {
	round++;
	petImage = new Image(imagePathes[(round - 1) % IMAGE_COUNT + 1]);
	RestartRound();
}

void XonixManager::RestartRound() {
	isAWin = false;
	isGameOver = false;
	capturedFieldPercentage = 0;

	mainCircle.SetDirection(Direction::None);
	for (int i = 0; i < fieldHeight; i++) {
		for (int j = 0; j < fieldWidth; j++) {
			if (i == 0 || j == 0 || i == fieldHeight - 1 || j == fieldWidth - 1)
				fieldCells[i][j] = CAPTURED;
			else fieldCells[i][j] = EMPTY;
		}
	}

	mainCircle = MainCircle(fieldWidth / 2, fieldHeight - 1, CIRCLE_RADIUS);

	srand((unsigned)time(NULL)); // С каждым запуском будут генерироваться разные числа.
	enemyCircles.clear();
	for (int i = 0; i < enemyCount; i++) {
		enemyCircles.push_back(EnemyCircle::GetRandomCircle(CIRCLE_RADIUS, Rect(1, 1, fieldWidth - 2, fieldHeight - 2)));
	}
}

bool XonixManager::MoveCircles(HDC hdc) {
	if (isGameOver) return false;
	
	bool newRound = false;

	Direction direction = mainCircle.GetDirection();
	bool finishMovement = false;
	int prevX = mainCircle.GetX();
	int prevY = mainCircle.GetY();

	mainCircle.MoveWithinTheBounds(Rect(0, 0, fieldWidth - 1, fieldHeight - 1));
	switch (fieldCells[mainCircle.GetY()][mainCircle.GetX()])
	{
	case EMPTY:
	{
		fieldCells[mainCircle.GetY()][mainCircle.GetX()] = TRAIL;
		break;
	}
	case CAPTURED:
	{
		if (fieldCells[prevY][prevX] == TRAIL) {
			mainCircle.SetDirection(Direction::None);

			for (size_t i = 0; i < enemyCircles.size(); i++)
				CheckCell(enemyCircles[i].GetX(), enemyCircles[i].GetY());

			UpdateField();
			finishMovement = true;

			if (capturedFieldPercentage >= capturedFieldPercentageToWin)
			{
				isAWin = true;
				
				for (int i = 0; i < fieldHeight; i++) {
					for (int j = 0; j < fieldWidth; j++)
					{
						if (fieldCells[i][j] != CAPTURED) {
							fieldCells[i][j] = CAPTURED;
						}
					}
				}
				capturedFieldPercentage = 100;
			}
		}
		break;
	}
	case TRAIL:
	{
		lifeCount--;
		if (lifeCount == 0) {
			isGameOver = true;
		}
		else {
			newRound = true;
			RestartRound();
		}
		break;
	}
	}

	Color color = mainCircle.GetColor();
	SolidBrush brush(color);
	Color darkerColor(max(color.GetR() - 100, 0),
		max(color.GetG() - 100, 0), max(color.GetB() - 100, 0));
	Pen pen(darkerColor, 1);
	SolidBrush darkBrush(darkerColor);

	Graphics graphics(hdc);
	
	graphics.FillRectangle(&darkBrush, x0 + prevX * CELL_SIZE + 1,
		y0 + prevY * CELL_SIZE + 1,	mainCircle.GetRadius() * 2,
		mainCircle.GetRadius() * 2);

	graphics.FillRectangle(&darkBrush, x0 + mainCircle.GetX() * CELL_SIZE + 1,
		y0 + mainCircle.GetY() * CELL_SIZE + 1,
		mainCircle.GetRadius() * 2, mainCircle.GetRadius() * 2);
	graphics.FillEllipse(&brush, x0 + mainCircle.GetX() * CELL_SIZE + 1,
		y0 + mainCircle.GetY() * CELL_SIZE + 1,
		mainCircle.GetRadius() * 2, mainCircle.GetRadius() * 2);

	for (size_t i = 0; i < enemyCircles.size(); i++) {
		Point previousPosition = enemyCircles[i].GetPosition();
		enemyCircles[i].MoveWithinTheBounds(Rect(0, 0, fieldWidth - 1, fieldHeight - 1));

		// Проверяем, не находится ли шарик на границе
		// со своей захваченной территорией. В этом случае корректируем
		// положение для отскока от границы. 
		if ((fieldCells[previousPosition.Y][enemyCircles[i].GetX()] == CAPTURED))
		{
			enemyCircles[i].SetX(previousPosition.X);
			enemyCircles[i].ReverseXDirection();
		} 

		if ((fieldCells[enemyCircles[i].GetY()][enemyCircles[i].GetX()] == CAPTURED))
		{
			enemyCircles[i].SetY(previousPosition.Y);
			enemyCircles[i].ReverseYDirection();
		}

		DrawCircle(hdc, enemyCircles[i], previousPosition);
		
		if (fieldCells[enemyCircles[i].GetY()][enemyCircles[i].GetX()] == TRAIL) {
			if (lifeCount > 0) {
				lifeCount--;
				if (lifeCount == 0) {
					isGameOver = true;
				}
				else {
					newRound = true;
					RestartRound();
				}
			}
		}
	}
	
	return finishMovement || newRound || isGameOver;
}

void XonixManager::OnPaint(HDC hdc) {
	Graphics graphics(hdc);

	graphics.DrawImage(petImage, x0, y0, imageWidth, imageHeight);

	if(isGameOver) return;

	LinearGradientBrush borderBrush(Rect (x0, y0, imageWidth, imageHeight), 
		Color(240, 190, 40), Color(250, 230, 80), LinearGradientModeForwardDiagonal);

	SolidBrush backgroundBrush(Color(255, 50, 80));
	for (int i = 0; i < fieldHeight; i++)
	{
		for (int j = 0; j < fieldWidth; j++) {
			if (i == 0 || j == 0 || i == fieldHeight - 1 || j == fieldWidth - 1) {
				graphics.FillRectangle(&borderBrush,
					Rect(x0 + j * CELL_SIZE, y0 + i * CELL_SIZE, CELL_SIZE, CELL_SIZE));
			}
			else if (fieldCells[i][j] == EMPTY)
			{
				graphics.FillRectangle(&backgroundBrush,
					Rect(x0 + j * CELL_SIZE, y0 + i * CELL_SIZE, CELL_SIZE, CELL_SIZE));
			}
		}
	}
}

void XonixManager::DrawCircle(HDC hdc, SimpleCircle circle, Point previousPosition)
{
	Graphics graphics(hdc);

	// Стираем предыдущий круг.
	Color backgroundColor = Color(255, 50, 80);
	SolidBrush bbrush(backgroundColor);
	Pen bpen(backgroundColor, 1);
	graphics.FillEllipse(&bbrush, x0 + previousPosition.X * CELL_SIZE, y0 + previousPosition.Y * CELL_SIZE,
		circle.GetRadius() * 2 + 2, circle.GetRadius() * 2 + 2);
	
	// Рисуем новый круг.
	Color color = circle.GetColor();
	SolidBrush brush(color);
	graphics.FillEllipse(&brush, x0 + circle.GetX() * CELL_SIZE + 1, y0 + circle.GetY() * CELL_SIZE + 1,
		circle.GetRadius() * 2, circle.GetRadius() * 2);
	
	// Контур делаем темнее.
	Color darkerColor(max(color.GetR() - 100, 0),
		max(color.GetG() - 100, 0), max(color.GetB() - 100, 0));
	Pen pen(darkerColor, 1);
	graphics.DrawEllipse(&pen, x0 + circle.GetX() * CELL_SIZE + 1, y0 + circle.GetY() * CELL_SIZE + 1,
		circle.GetRadius() * 2, circle.GetRadius() * 2);
}

void XonixManager::CheckCell(int x, int y)
{
	stack<int> CordinateStack;

	CordinateStack.push(x);
	CordinateStack.push(y);
	while (CordinateStack.empty() == false) {
		if (fieldCells[y][x] == EMPTY) {
			fieldCells[y][x] = ENEMY;
		}
		if (fieldCells[y - 1][x] == EMPTY)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			y--;
		}
		else if (fieldCells[y + 1][x] == EMPTY)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			y++;
		}
		else if (fieldCells[y][x - 1] == EMPTY)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			x--;
		}
		else if (fieldCells[y][x + 1] == EMPTY)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			x++;
		}
		else
		{
			y = CordinateStack.top();
			CordinateStack.pop();
			x = CordinateStack.top();
			CordinateStack.pop();
		}
	}
}

void XonixManager::UpdateField()
{
	int enemyCellCount = 0;
	for (int i = 0; i < fieldHeight; i++) {
		for (int j = 0; j < fieldWidth; j++) {
			if (fieldCells[i][j] == ENEMY) {
				fieldCells[i][j] = EMPTY;
				enemyCellCount++;
			}
			else {
				fieldCells[i][j] = CAPTURED;
			}
		}
	}

	capturedFieldPercentage = 100 - ((float)enemyCellCount /
		((fieldWidth - 1) * (fieldHeight - 1))) * 100;
}


void XonixManager::ZoomImageToFitRect(
	int* imageWidth, int *imageHeight, int rectWidth, int rectHeight)
{
	// Пропорционально масштабируем картинку.
	double scaleX = (double)rectWidth / *imageWidth;

	double newImageWidth = *imageWidth * scaleX;
	double newImageHeight = *imageHeight * scaleX;

	double scaleY = (double)rectHeight / newImageHeight;

	if (scaleY < 1)
	{
		newImageWidth *= scaleY;
		newImageHeight *= scaleY;
	}

	*imageWidth = (int)newImageWidth;
	*imageHeight = (int)newImageHeight;
}
