#pragma once
#include "SimpleCircle.h"

class MainCircle :public SimpleCircle
{
private:
	const Color COLOR = Color(0, 200, 230); // голубой цвет
public:
	MainCircle();
	MainCircle(int x, int y);
	MainCircle(MainCircle&);
	~MainCircle();
};
