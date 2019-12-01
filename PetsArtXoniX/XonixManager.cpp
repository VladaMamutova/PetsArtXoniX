﻿#include "XonixManager.h"
#pragma comment(lib, "GdiPlus.lib")

XonixManager::XonixManager()
{
	level = 1;

	// Определяем словарь картинок.
	wchar_t name[LEVEL_COUNT][50] = { L"yorkshire-terrier" };
	wchar_t path[50];
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

void XonixManager::StartNewGame() {
	level = 1;
	LoadPetImage();
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

void XonixManager::OnPaint(HDC hdc, RECT rect) {
	// Пропорционально масштабируем картинку.
	Graphics graph(hdc);

	double imageWidth = petImage->GetWidth();
	double imageHeight = petImage->GetHeight();
	double windowWidth = rect.right - rect.left;
	double windowHeight = rect.bottom - rect.top;

	double scaleX = windowWidth / imageWidth;

	double newWidth = imageWidth * scaleX;
	double newHeight = imageHeight * scaleX;

	double scaleY = windowHeight / newHeight;

	if (scaleY < 1)
	{
		newWidth *= scaleY;
		newHeight *= scaleY;
	}

	graph.DrawImage(petImage, (int)((rect.right - newWidth) / 2),
		(int)rect.top,
		(int)newWidth, (int)newHeight);
}