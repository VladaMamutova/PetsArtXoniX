#include "XonixManager.h"
#include "FieldCuttingHelper.h"
#include <math.h>
#include <time.h>

#pragma comment(lib, "GdiPlus.lib")

using namespace Gdiplus;

XonixManager::XonixManager()
{
	level = 1;

	// Определяем словарь картинок.
	wchar_t name[LEVEL_COUNT][50] = { L"yorkshire-terrier" };
	for (int i = 1; i <= LEVEL_COUNT; i++) {
		swprintf_s(imagePathes[i].Filled, L"%ls.jpg", name[i - 1]);
		swprintf_s(imagePathes[i].Outline, L"%ls-outline.jpg", name[i - 1]);
	}
}

XonixManager::~XonixManager()
{
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
	width = petImage->GetWidth();
	height = petImage->GetHeight();
	ZoomImageToFitRect(&width, &height,
		windowWidth - 2 * FIELD_MARGIN,
		windowHeight - 2 * FIELD_MARGIN);

	width -= 2 * BORDER_THICKNESS;
	height -= 2 * BORDER_THICKNESS;

	// Находим верхний левый отступ от краёв окна так,
	// чтобы картинка размещалась посередине.
	x0 = (windowWidth - width) / 2;
	y0 = (windowHeight - height) / 2;

	InitMainCircle(width / 2 - mainCircle.GetRadius(), height);
	enemyCount = 1;
	InitEnemyCircles(Gdiplus::Rect(0, 0, width, height));
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

void XonixManager::InitEnemyCircles(Gdiplus::Rect bounds) {
	srand((unsigned)time(NULL)); // С каждым запуском будут генерироваться разные числа.
	for (int i = 0; i < enemyCount; i++) {
		int x = bounds.GetLeft() + rand() % bounds.Width;
		int y = bounds.GetTop() + rand() % bounds.Height;
		enemyCircles.push_back(EnemyCircle(x, y, CIRCLE_RADIUS));
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

void XonixManager::AddPointToMainCirclePath() {
	// Добавляем точку текущего положения шарика.
	mainCirclePath.push_back(Point(mainCircle.GetX(), mainCircle.GetY()));
}

bool XonixManager::MoveCircle(HDC hdc) {
	Direction direction = mainCircle.GetDirection();
	int x = mainCircle.GetX();
	int y = mainCircle.GetY();
	mainCircle.MoveWithinTheBounds(Gdiplus::Rect(0, 0, width, height));
	
	bool finishMovement = false;
	
	// Шарик прекратил движение и прошёл путь ненулевой длины..
	if (mainCircle.GetDirection() == Direction::None && mainCirclePath.size() > 0) {
		// Фиксируем точку положения шарика.
		mainCirclePath.push_back(Point(mainCircle.GetX(), mainCircle.GetY()));
		
		// Получаем захваченные шариком прямоугольные области.
		vector<Rect> newCapturedRects = FieldCuttingHelper::SplitIntoRects(mainCirclePath);
		for (size_t i = 0; i < newCapturedRects.size(); i++) {
			capturedField.push_back(newCapturedRects[i]);
		}
		// Очищаем точки пути.
		mainCirclePath.clear();
		finishMovement = true;
	}

	DrawCircle(hdc, mainCircle);
	for (size_t i = 0; i < enemyCircles.size(); i++) {
		DrawCircle(hdc, enemyCircles[i]);
	}

	return finishMovement;
}

void XonixManager::OnPaint(HDC hdc) {
	Graphics graphics(hdc);

	graphics.DrawImage(petImageOutline, x0, y0, width, height);

	// Атрибуты для указания, как будет отрисовываться изображение.
	//ImageAttributes imAtt;
	//imAtt.SetWrapMode(WrapModeTileFlipXY);
	//graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
	//graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

	//Rect zoomRect(x, y, 300, 300);
	//graphics.DrawImage(petImage, zoomRect, 0, 0, 
	//	300 * imageWidth/newWidth, 300 * imageHeight/ newHeight, UnitPixel, &imAtt);

	for (size_t i = 0; i < capturedField.size(); i++) {
		graphics.DrawImage(petImage, Rect(capturedField[i].X + x0, capturedField[i].Y + y0, capturedField[i].Width, capturedField[i].Height),
			capturedField[i].X * petImage->GetWidth() / width,
			capturedField[i].Y * petImage->GetHeight() / height,
			capturedField[i].Width * petImage->GetWidth() / width,
			capturedField[i].Height * petImage->GetHeight() / height, UnitPixel);
	}

	// Рисуем границу картинки.
	Rect borderRect(x0 - BORDER_THICKNESS, y0 - BORDER_THICKNESS,
		width + 2 * BORDER_THICKNESS, height + 2 * BORDER_THICKNESS);
	LinearGradientBrush brush(borderRect, Color(255, 150, 0), Color(255, 170, 0), LinearGradientModeForwardDiagonal);
	Pen pen(&brush, (REAL)BORDER_THICKNESS);
	pen.SetAlignment(PenAlignmentInset);
	graphics.DrawRectangle(&pen, borderRect);
}

void XonixManager::DrawCircle(HDC hdc, SimpleCircle circle)
{
	Graphics graphics(hdc);

	Color color = circle.GetColor();
	SolidBrush brush(color);
	graphics.FillEllipse(&brush, x0 + circle.GetX(), y0 + circle.GetY(),
		circle.GetRadius() * 2, circle.GetRadius() * 2);

	Color darkerColor(max(color.GetR() - 100, 0),
		max(color.GetG() - 100, 0), max(color.GetB() - 100, 0));
	Pen pen(darkerColor, 1);
	graphics.DrawEllipse(&pen, x0 + circle.GetX(), y0 + circle.GetY(),
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
