#include "XonixManager.h"
#include <math.h>
#include <time.h>
#include <stack>

using namespace Gdiplus;

#pragma comment(lib, "GdiPlus.lib")

XonixManager::XonixManager(Rect gameRect)
{
	level = 1;
	enemyCount = 1;
	speed = SPEED::AVERAGE;
	isGameOver = false;

	// Определяем словарь картинок.
	wchar_t name[LEVEL_COUNT][50] = { L"yorkshire-terrier" };
	for (int i = 1; i <= LEVEL_COUNT; i++) {
		swprintf_s(imagePathes[i].Filled, L"%ls.jpg", name[i - 1]);
		swprintf_s(imagePathes[i].Outline, L"%ls.jpg", name[i - 1]);
	}
	LoadPetImage();

	// По размеру окна вычисляем размер картинки.
	width = petImage->GetWidth();
	height = petImage->GetHeight();
	ZoomImageToFitRect(&width, &height,
		gameRect.Width,
		gameRect.Height);

	width -= 2 * BORDER_THICKNESS;
	height -= 2 * BORDER_THICKNESS;

	// Корректируем размеры с учётом размера ячейки поля.
	width -= width % CELL_SIZE;
	height -= height % CELL_SIZE;

	// Находим верхний левый отступ от краёв окна так,
	// чтобы картинка размещалась посередине.
	x0 = (gameRect.Width - width) / 2 + gameRect.GetLeft();
	y0 = (gameRect.Height - height) / 2 + gameRect.GetTop();

	// Вычисляем размер поля по количеству ячеек.
	fieldWidth = width / CELL_SIZE;
	fieldHeight = height / CELL_SIZE;

	// Создаём поле с пустыми ячейками.
	fieldCells = new int*[fieldHeight];
	for (int i = 0; i < fieldHeight; ++i)
		fieldCells[i] = new int[fieldWidth];

	for (int i = 0; i < fieldHeight; i++) {
		for (int j = 0; j < fieldWidth; j++) {
			if (i == 0 || j == 0 || i == fieldHeight - 1 || j == fieldWidth - 1)
				fieldCells[i][j] = 1;
			else fieldCells[i][j] = 0;//EMPTY;
		}
	}
}

XonixManager::~XonixManager()
{
	for (int i = 0; i < fieldHeight; ++i)
		delete[] fieldCells[i];
	delete[] fieldCells;

	if (petImage) {
		delete petImage;
	}
	if (petImageOutline) {
		delete petImageOutline;
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

bool XonixManager::GetEnemyCount()
{
	return enemyCount;
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

void XonixManager::StartNewGame() {
	isAWin = false;
	isGameOver = false;
	capturedFieldPercentage = 0;

	mainCircle.SetDirection(Direction::None);
	for (int i = 0; i < fieldHeight; i++) {
		for (int j = 0; j < fieldWidth; j++) {
			if (i == 0 || j == 0 || i == fieldHeight - 1 || j == fieldWidth - 1)
				fieldCells[i][j] = 1;
			else fieldCells[i][j] = 0;//EMPTY;
		}
	}

	InitMainCircle(fieldWidth / 2, fieldHeight - 1);
	InitEnemyCircles(Rect(1, 1, fieldWidth - 2, fieldHeight - 2));
}

void XonixManager::LoadPetImage()
{
	// Получаем путь к текущей директории.
	wchar_t path[500];
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

	wchar_t imagePath[500];
	swprintf_s(imagePath, L"%ls\\img\\%ls", path, imagePathes[level].Filled);
	petImage = new Image(imagePath);
	swprintf_s(imagePath, L"%ls\\img\\%ls", path, imagePathes[level].Outline);
	petImageOutline = new Image(imagePath);
}

void XonixManager::InitMainCircle(int x, int y) {
	mainCircle = MainCircle(x, y, CIRCLE_RADIUS);
}

void XonixManager::InitEnemyCircles(Rect bounds) {
	srand((unsigned)time(NULL)); // С каждым запуском будут генерироваться разные числа.
	enemyCircles.clear();
	for (int i = 0; i < enemyCount; i++) {
		enemyCircles.push_back(EnemyCircle::GetRandomCircle(CIRCLE_RADIUS, bounds));
	}
}

void XonixManager::SetTopMove() {
	mainCircle.SetDirection(Direction::Up);
	AddPointToMainCirclePath();
}

void XonixManager::SetBottomMove() {
	mainCircle.SetDirection(Direction::Down);
	AddPointToMainCirclePath();
}

void XonixManager::SetLeftMove() {
	mainCircle.SetDirection(Direction::Left);
	AddPointToMainCirclePath();
}

void XonixManager::SetRightMove() {
	mainCircle.SetDirection(Direction::Right);
	AddPointToMainCirclePath();
}

void XonixManager::SetEnemyCount(int enemyCount)
{
	this->enemyCount = enemyCount;
}

void XonixManager::SetSpeed(SPEED speed)
{
	this->speed = speed;
}

float XonixManager::GetCapturedFieldPersentage()
{
	return capturedFieldPercentage;
}

void XonixManager::AddPointToMainCirclePath() {
	// Добавляем точку текущего положения шарика.
	mainCirclePath.push_back(Point(max(mainCircle.GetX(), 0), max(mainCircle.GetY(), 0)));
}

bool XonixManager::MoveCircle(HDC hdc) {
	if (isGameOver) return false;

	Direction direction = mainCircle.GetDirection();
	bool finishMovement = false;
	int prevX = mainCircle.GetX();
	int prevY = mainCircle.GetY();

	mainCircle.MoveWithinTheBounds(Rect(0, 0, fieldWidth - 1, fieldHeight - 1));
	switch (fieldCells[mainCircle.GetY()][mainCircle.GetX()])
	{
	case 0:
	{
		fieldCells[mainCircle.GetY()][mainCircle.GetX()] = 2;
		break;
	}
	case 1:
	{
		if (fieldCells[prevY][prevX] == 2) {
			mainCircle.SetDirection(Direction::None);

			for (size_t i = 0; i < enemyCircles.size(); i++)
				CheckCell(enemyCircles[i].GetX(), enemyCircles[i].GetY());

			UpdateField();
			finishMovement = true;

			if (capturedFieldPercentage >= capturedFieldPercentageToWin)
			{
				isAWin = true;
				level++;
			}
		}
		break;
	}
	case 2:
	{
		isGameOver = true;
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
	graphics.FillRectangle(&darkBrush, x0 + prevX * CELL_SIZE, y0 + prevY * CELL_SIZE,
		mainCircle.GetRadius() * 2 + 0, mainCircle.GetRadius() * 2 + 0);

	graphics.FillRectangle(&darkBrush, x0 + mainCircle.GetX() * CELL_SIZE, y0 + mainCircle.GetY() * CELL_SIZE,
		mainCircle.GetRadius() * 2 , mainCircle.GetRadius() * 2 );
	graphics.FillEllipse(&brush, x0 + mainCircle.GetX() * CELL_SIZE, y0 + mainCircle.GetY() * CELL_SIZE,
		mainCircle.GetRadius() * 2, mainCircle.GetRadius() * 2);
/*
	graphics.DrawEllipse(&pen, x0 + mainCircle.GetX() * CELL_SIZE, y0 + mainCircle.GetY() * CELL_SIZE,
		mainCircle.GetRadius() * 2, mainCircle.GetRadius() * 2);*/

	for (size_t i = 0; i < enemyCircles.size(); i++) {
		Point previousPosition = enemyCircles[i].GetPosition();
		enemyCircles[i].MoveWithinTheBounds(Rect(0, 0, fieldWidth - 1, fieldHeight - 1));

				
		if ((fieldCells[previousPosition.Y ][enemyCircles[i].GetX()] == 1))
		{
			enemyCircles[i].SetX(previousPosition.X);
			enemyCircles[i].ReverseXDirection();
		} 

		if ((fieldCells[enemyCircles[i].GetY()][enemyCircles[i].GetX()] == 1))
		{
			enemyCircles[i].SetY(previousPosition.Y);
			enemyCircles[i].ReverseYDirection();
		}

		DrawCircle(hdc, enemyCircles[i], previousPosition);
		
		if (fieldCells[enemyCircles[i].GetY()][enemyCircles[i].GetX()] == 2) {
			isGameOver = true;
		}
	}
	
	return finishMovement || isGameOver;
}

void XonixManager::OnPaint(HDC hdc) {
	Graphics graphics(hdc);

	graphics.DrawImage(petImageOutline, x0, y0, width, height);

	if(isGameOver) return;

	for (int i = 0; i < fieldHeight; i++)
	{
		for (int j = 0; j < fieldWidth; j++) {
			if (i == 0 || j == 0 || i == fieldHeight - 1 || j == fieldWidth - 1) {

			}
			else if (fieldCells[i][j] == 0)
			{
				Color color = Color(255, 50, 80);
				SolidBrush brush(color);
				graphics.FillRectangle(&brush, Rect(x0 + j * CELL_SIZE, y0 + i * CELL_SIZE, CELL_SIZE, CELL_SIZE));
			}
		}
	}
	Rect borderRect(x0, y0, width, height);

	LinearGradientBrush brush(borderRect, Color(240, 190, 40), Color(250, 230, 80), LinearGradientModeForwardDiagonal);
	Pen pen(&brush, (REAL)BORDER_THICKNESS);
	pen.SetAlignment(PenAlignmentInset);
	graphics.DrawRectangle(&pen, borderRect);
}

void XonixManager::DrawCircle(HDC hdc, SimpleCircle circle, Point previousPosition)
{
	Graphics graphics(hdc);

	// Стираем предыдущий круг.
	Color backgroundColor = Color(255, 50, 80);
	SolidBrush bbrush(backgroundColor);
	Pen bpen(backgroundColor, 1);
	graphics.FillEllipse(&bbrush, x0 + previousPosition.X * CELL_SIZE - 1, y0 + previousPosition.Y * CELL_SIZE - 1,
		circle.GetRadius() * 2 + 2, circle.GetRadius() * 2 + 2);
	
	// Рисуем новый круг.
	Color color = circle.GetColor();
	SolidBrush brush(color);
	graphics.FillEllipse(&brush, x0 + circle.GetX() * CELL_SIZE, y0 + circle.GetY() * CELL_SIZE,
		circle.GetRadius() * 2, circle.GetRadius() * 2);
	
	// Контур делаем темнее.
	Color darkerColor(max(color.GetR() - 100, 0),
		max(color.GetG() - 100, 0), max(color.GetB() - 100, 0));
	Pen pen(darkerColor, 1);
	graphics.DrawEllipse(&pen, x0 + circle.GetX() * CELL_SIZE, y0 + circle.GetY() * CELL_SIZE,
		circle.GetRadius() * 2, circle.GetRadius() * 2);
}

void XonixManager::CheckCell(int x, int y)
{
	stack<int> CordinateStack;

	CordinateStack.push(x);
	CordinateStack.push(y);
	while (CordinateStack.empty() == false) {
		if (fieldCells[y][x] == 0) 
			fieldCells[y][x] = -1;
		if (fieldCells[y - 1][x] == 0)
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			y--;
		}
		else if (fieldCells[y + 1][x] == 0) // 0
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			y++;
		}
		else if (fieldCells[y][x - 1] == 0) //0
		{
			CordinateStack.push(x);
			CordinateStack.push(y);
			x--;
		}
		else if (fieldCells[y][x + 1] == 0)
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
	int ColEnemyCell = 0;
	for (int i = 0; i < fieldHeight; i++)
		for (int j = 0; j < fieldWidth; j++)
			if (fieldCells[i][j] == -1)
			{
				fieldCells[i][j] = 0;
				ColEnemyCell++;
			}
			else fieldCells[i][j] = 1;

	capturedFieldPercentage = 100 - ((float)ColEnemyCell /
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
