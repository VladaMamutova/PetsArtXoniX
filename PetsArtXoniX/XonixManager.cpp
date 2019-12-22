#include "XonixManager.h"
#include <math.h>
#include <time.h>

#pragma comment(lib, "GdiPlus.lib")

using namespace Gdiplus;

XonixManager::XonixManager()
{
	level = 1;
	mainCircle.SetRadius(CELL_SIZE);

	// Определяем словарь картинок.
	wchar_t name[LEVEL_COUNT][50] = { L"yorkshire-terrier" };
	for (int i = 1; i <= LEVEL_COUNT; i++) {
		swprintf_s(imagePathes[i].Filled, L"%ls.jpg", name[i - 1]);
		swprintf_s(imagePathes[i].Outline, L"%ls-outline.jpg", name[i - 1]);
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

void XonixManager::StartNewGame(int windowWidth, int windowHeight) {
	level = 1;
	LoadPetImage();

	// По размеру окна вычисляем размер картинки.
	int imageWidth = petImage->GetWidth();
	int imageHeight = petImage->GetHeight();
	ZoomImageToFitRect(&imageWidth, &imageHeight,
		windowWidth - 2 * FIELD_MARGIN,
		windowHeight - 2 * FIELD_MARGIN);

	// Корректируем размеры с учётом размера ячейки поля.
	imageWidth -= imageWidth % CELL_SIZE;
	imageHeight -= imageHeight % CELL_SIZE;

	// Находим верхний правый отступ от краёв окна так,
	// чтобы картинка размещалась посередине.
	x0 = (windowWidth - imageWidth) / 2;
	y0 = (windowHeight - imageHeight) / 2;

	InitMainCircle(x0 + imageWidth / 2 - mainCircle.GetRadius(), y0 + imageHeight);
	enemyCount = 1;
	InitEnemyCircles(Rect(x0, y0, imageWidth, imageHeight));

	// Вычисляем размер поля по количеству ячеек.
	fieldWidth = imageWidth / CELL_SIZE;
	fieldHeight = imageHeight / CELL_SIZE;

	// Создаём поле с пустыми ячейками.
	fieldCells = new int*[fieldHeight];
	for (int i = 0; i < fieldHeight; ++i)
		fieldCells[i] = new int[fieldWidth];

	for (int i = 0; i < fieldHeight; i++) {
		for (int j = 0; j < fieldWidth; j++) {
			fieldCells[i][j] = EMPTY;
		}
	}
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
	mainCircle.SetX(x);
	mainCircle.SetY(y);
}

void XonixManager::InitEnemyCircles(Rect bounds) {
	srand(time(NULL)); // С каждым запуском будут генерироваться разные числа.
	for (int i = 0; i < enemyCount; i++) {
		int x = bounds.GetLeft() + rand() % bounds.Width;
		int y = bounds.GetTop() + rand() % bounds.Height;
		enemyCircles.push_back(EnemyCircle(x, y));
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

void XonixManager::MoveCircle(HDC hdc) {
	Direction direction = mainCircle.GetDirection();
	int circleX = mainCircle.GetX();
	int circleY = mainCircle.GetY();
	mainCircle.MoveWithinTheBounds(Gdiplus::Rect(x0, y0,
		x0 + fieldWidth * CELL_SIZE, y0 + fieldHeight * CELL_SIZE));

	// Если шарик начал движение, то фиксируем точку начала.
	if (direction == Direction::None && mainCircle.GetDirection() != Direction::None) {

	}

	// Если шарик двигался и прекратил движение, то 
	if (direction != Direction::None && mainCircle.GetDirection() == Direction::None) {
		
	}

	DrawCircle(hdc, mainCircle);
	for (int i = 0; i < enemyCircles.size(); i++) {
		DrawCircle(hdc, enemyCircles[i]);
	}
}

void XonixManager::OnPaint(HDC hdc, RECT rect) {
	Graphics graphics(hdc);

	graphics.DrawImage(petImageOutline, x0, y0, fieldWidth * CELL_SIZE, fieldHeight * CELL_SIZE);

	// Атрибуты для указания, как будет отрисовываться изображение.
	//ImageAttributes imAtt;
	//imAtt.SetWrapMode(WrapModeTileFlipXY);
	//graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
	//graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

	//Rect zoomRect(x, y, 300, 300);
	//graphics.DrawImage(petImage, zoomRect, 0, 0, 
	//	300 * imageWidth/newWidth, 300 * imageHeight/ newHeight, UnitPixel, &imAtt);

	for (int i = 0; i < fieldHeight; i++)
	{
		for (int j = 0; j < fieldWidth; j++) {
			if (fieldCells[i][j] == FILLED)
			{
				Gdiplus::Rect zoomRect(x0 + j * CELL_SIZE, y0 + i * CELL_SIZE, CELL_SIZE, CELL_SIZE);
				graphics.DrawImage(petImage, zoomRect, 0, 0, CELL_SIZE * petImage->GetWidth() / fieldWidth,
					CELL_SIZE * petImage->GetHeight() / fieldHeight, UnitPixel);
			}
		}
	}

	Rect borderRect(x0 - 2 * mainCircle.GetRadius(), y0 - 2 * mainCircle.GetRadius(),
		fieldWidth * CELL_SIZE + 4 * mainCircle.GetRadius(), fieldHeight * CELL_SIZE + 4 * mainCircle.GetRadius());
	LinearGradientBrush brush(borderRect, Color(255, 150, 0), Color(255, 170, 0), LinearGradientModeForwardDiagonal);
	Pen pen(&brush, (REAL)2 * mainCircle.GetRadius());
	pen.SetAlignment(PenAlignmentInset);
	graphics.DrawRectangle(&pen, borderRect);
}

void XonixManager::DrawCircle(HDC hdc, SimpleCircle circle)
{
	Graphics graphics(hdc);

	Color color = circle.GetColor();
	SolidBrush brush(color);
	graphics.FillEllipse(&brush, circle.GetX(), circle.GetY(),
		circle.GetRadius() * 2, circle.GetRadius() * 2);

	Color darkerColor(max(color.GetR() - 100, 0),
		max(color.GetG() - 100, 0), max(color.GetB() - 100, 0));
	Pen pen(darkerColor, 1);
	graphics.DrawEllipse(&pen, circle.GetX(), circle.GetY(),
		circle.GetRadius() * 2, circle.GetRadius() * 2);
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
