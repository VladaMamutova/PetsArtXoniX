#pragma once
#include "SimpleCircle.h"
#include "IMovable.h"
#include "Direction.h"

class MainCircle :public SimpleCircle, public IMovable
{
private:
	const Color COLOR = Color(0, 200, 230); // голубой цвет
	Direction direction = Direction::None; // текущее направление движения шарика
public:
	MainCircle();
	MainCircle(int x, int y);
	MainCircle(int x, int y, int radius);
	MainCircle(const MainCircle&);
	MainCircle& operator=(const MainCircle&);
	void SetDirection(Direction direction);
	Direction GetDirection();
	virtual void MoveWithinTheBounds(Rect bounds);
	~MainCircle();
};
