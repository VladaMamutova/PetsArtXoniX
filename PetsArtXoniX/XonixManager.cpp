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
	for (int i = 0; i < width; ++i)
		delete[] field[i];
	delete[] field;

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

	windowWidth -= FIELD_MARGIN * 2;
	windowHeight -= FIELD_MARGIN * 2;
	
	// Находим размеры картинки и поля при пропорциональном масшабировании.
	double scaleX = (double)windowWidth / petImage->GetWidth();

	double imageWidth = petImage->GetWidth() * scaleX;
	double imageHeight = petImage->GetHeight() * scaleX;

	double scaleY = (double)windowHeight / imageHeight;

	if (scaleY < 1)
	{
		imageWidth *= scaleY;
		imageHeight *= scaleY;
	}

	width = (int)imageWidth;
	height = (int)imageHeight;

	x0 = (windowWidth + FIELD_MARGIN * 2 - width) / 2;
	y0 = (windowHeight + FIELD_MARGIN * 2 - height) / 2;

	field = new int*[width];
	for (int i = 0; i < width; ++i)
		field[i] = new int[height];
	
	InitMainCircle(x0 + width / 2 - mainCircle.GetRadius(), y0 + height);
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
	startMovementPoint.X = x;
	startMovementPoint.Y = y;
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

void XonixManager::MoveCircle(HDC hdc) {
	Direction direction = mainCircle.GetDirection();
	int circleX = mainCircle.GetX();
	int circleY = mainCircle.GetY();
	mainCircle.MoveWithinTheBounds(Gdiplus::Rect(x0, y0, x0 + width, y0 + height));

	// Если шарик начал движение, то фиксируем точку начала.
	if (direction == Direction::None && mainCircle.GetDirection() != Direction::None) {
		startMovementPoint.X = circleX;
		startMovementPoint.Y = circleY;
	}

	// Если шарик двигался и прекратил движение, то 
	if (direction != Direction::None && mainCircle.GetDirection() == Direction::None) {
		startMovementPoint.X = circleX;
		startMovementPoint.Y = circleY;
	}

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

	Gdiplus::Rect zoomRect(x0, y0, 300, 300);
	graphics.DrawImage(petImage, zoomRect, 0, 0, 
		300 * petImage->GetWidth()/width, 300 * petImage->GetHeight()/ height, UnitPixel);
	Rect borderRect(x0 - 2 * mainCircle.GetRadius(), y0 - 2 * mainCircle.GetRadius(),
		width + 4 * mainCircle.GetRadius(), height + 4 * mainCircle.GetRadius());
	LinearGradientBrush brush(borderRect, Color(255, 150, 0), Color(255, 170, 0), LinearGradientModeForwardDiagonal);
	Pen pen(&brush, (REAL)2 * mainCircle.GetRadius());
	pen.SetAlignment(PenAlignmentInset);
	graphics.DrawRectangle(&pen, borderRect);
}
