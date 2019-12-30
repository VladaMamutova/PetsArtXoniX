#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>

class SimpleCircle
{
protected:
	const int DEFAULT_RADIUS = 10;
	const Gdiplus::Color DEFAULT_COLOR = Gdiplus::Color(0, 0, 0);

	int x;
	int y;
	int radius;
	Gdiplus::Color color;
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
	Gdiplus::Color GetColor();
	Gdiplus::Point GetPosition();
	void SetX(int x);
	void SetY(int y);
	void SetRadius(int radius);
	void SetColor(Gdiplus::Color color);
	void SetPosition(Gdiplus::Point position);
};
