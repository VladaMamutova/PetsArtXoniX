#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>

using namespace Gdiplus;

class SimpleCircle
{
private:
	const int DEFAULT_RADIUS = 10;
	const Color DEFAULT_COLOR = Color(0, 0, 0);
protected:
	int x;
	int y;
	int radius;
	Color color;
public:
	SimpleCircle();
	SimpleCircle(int x, int y);
	SimpleCircle(int x, int y, int radius);
	SimpleCircle(const SimpleCircle&);
	SimpleCircle& operator=(const SimpleCircle&);
	~SimpleCircle();
	int GetX();
	int GetY();
	int GetRadius();
	Color GetColor();
	void SetX(int x);
	void SetY(int y);
	void SetRadius(int radius);
	void SetColor(Color color);
};
