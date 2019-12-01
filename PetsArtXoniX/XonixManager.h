#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>
#include <string>
#include <map>
#include "PairImages.h"

using namespace std;
using namespace Gdiplus;

#define LEVEL_COUNT 1

class XonixManager
{
private:
	Image *petImage; // Текущая цветная заполненная картинка.
	Image *petImageOutline; // Текущая картинка-контур.

	// Словарь картинок:
	// ключ - номер уровня,
	// значение - структура, содержащая пару картинок: заполненную и контур.
	map <int, PairImages> imagePathes;
	int level;
public:
	XonixManager();
	~XonixManager();
	void StartNewGame();
	void LoadPetImage();
	void OnPaint(HDC, RECT);
};

