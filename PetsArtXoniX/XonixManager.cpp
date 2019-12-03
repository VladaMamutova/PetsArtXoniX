#include "XonixManager.h"
#include <math.h>
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

void XonixManager::StartNewGame(Gdiplus::Rect rect) {
	level = 1;
	LoadPetImage();

	int padding = 30;
	double imageWidth = petImage->GetWidth();
	double imageHeight = petImage->GetHeight();
	double windowWidth = rect.Width - padding * 2;
	double windowHeight = rect.Height - padding * 2;

	double scaleX = windowWidth / imageWidth;

	double newWidth = imageWidth * scaleX;
	double newHeight = imageHeight * scaleX;

	double scaleY = windowHeight / newHeight;

	if (scaleY < 1)
	{
		newWidth *= scaleY;
		newHeight *= scaleY;
	}

	int x = (int)(rect.GetRight() - newWidth) / 2;
	int y = (int)(rect.GetBottom() - newHeight) / 2;

	InitMainCircle(x + (int)(newWidth / 2) - mainCircle.GetRadius(),
		y + (int)newHeight);
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

//void XonixManager::InitEnemyCircles() {
//	SimpleCircle mainCircleArea = mainCircle.getCircleArea();
//	circles = new ArrayList<>();
//	for (int i = 0; i < ENEMY_CIRCLES_COUNT; i++) {
//		EnemyCircle circle;
//		do {
//			circle = EnemyCircle.getRandomCircle(false);
//		} while (circle.isIntersect(mainCircleArea));
//		circles.add(circle);
//	}
//}

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

void XonixManager::MoveCircle(HDC hdc, RECT rect) {
	int padding = 30;
	double imageWidth = petImage->GetWidth();
	double imageHeight = petImage->GetHeight();
	double windowWidth = rect.right - rect.left - padding * 2;
	double windowHeight = rect.bottom - rect.top - padding * 2;

	double scaleX = windowWidth / imageWidth;

	double newWidth = imageWidth * scaleX;
	double newHeight = imageHeight * scaleX;

	double scaleY = windowHeight / newHeight;

	if (scaleY < 1)
	{
		newWidth *= scaleY;
		newHeight *= scaleY;
	}

	int x = (int)(rect.right - newWidth) / 2;
	int y = (int)(rect.bottom - newHeight) / 2;

	mainCircle.MoveWithinTheBounds(Gdiplus::Rect(x, y, x + (int)newWidth, y + (int)newHeight));
	Graphics graphics(hdc);
	Color color = mainCircle.GetColor();
	SolidBrush brush(color);
	graphics.FillEllipse(&brush, mainCircle.GetX(), mainCircle.GetY(),
		mainCircle.GetRadius() * 2, mainCircle.GetRadius() * 2);

	Color darkerColor(max(color.GetR() - 100, 0),
		max(color.GetG() - 100, 0), max(color.GetB() - 100, 0));
	Pen pen(darkerColor, 1);
	graphics.DrawEllipse(&pen, mainCircle.GetX(), mainCircle.GetY(),
		mainCircle.GetRadius() * 2, mainCircle.GetRadius() * 2);
}

void XonixManager::OnPaint(HDC hdc, RECT rect) {
	// Пропорционально масштабируем картинку.
	Graphics graphics(hdc);

	int padding = 30;
	double imageWidth = petImage->GetWidth();
	double imageHeight = petImage->GetHeight();
	double windowWidth = rect.right - rect.left - padding * 2;
	double windowHeight = rect.bottom - rect.top - padding * 2;

	double scaleX = windowWidth / imageWidth;

	double newWidth = imageWidth * scaleX;
	double newHeight = imageHeight * scaleX;

	double scaleY = windowHeight / newHeight;

	if (scaleY < 1)
	{
		newWidth *= scaleY;
		newHeight *= scaleY;
	}

	int x = (int)(rect.right - newWidth) / 2;
	int y = (int)(rect.bottom - newHeight) / 2;
	
	graphics.DrawImage(petImageOutline, x, y,
		(int)newWidth, (int)newHeight);

	// Атрибуты для указания, как будет отрисовываться изображение.
	//ImageAttributes imAtt;
	//imAtt.SetWrapMode(WrapModeTileFlipXY);
	//graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
	//graphics.SetPixelOffsetMode(PixelOffsetModeHalf);

	//Rect zoomRect(x, y, 300, 300);
	//graphics.DrawImage(petImage, zoomRect, 0, 0, 
	//	300 * imageWidth/newWidth, 300 * imageHeight/ newHeight, UnitPixel, &imAtt);

	Gdiplus::Rect zoomRect(x, y, 300, 300);
	graphics.DrawImage(petImage, zoomRect, 0, 0, 
		(int)(300 * imageWidth/newWidth), (int)(300 * imageHeight/ newHeight), UnitPixel);
	Rect borderRect(x - 2 * mainCircle.GetRadius(), y - 2 * mainCircle.GetRadius(),
		(int)newWidth + 4 * mainCircle.GetRadius(), (int)newHeight + 4 * mainCircle.GetRadius());
	LinearGradientBrush brush(borderRect, Color(255, 150, 0), Color(255, 170, 0), LinearGradientModeForwardDiagonal);
	Pen pen(&brush, (REAL)2 * mainCircle.GetRadius());
	pen.SetAlignment(PenAlignmentInset);
	graphics.DrawRectangle(&pen, borderRect);
}
