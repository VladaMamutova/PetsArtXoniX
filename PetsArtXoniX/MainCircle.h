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
	MainCircle(MainCircle&);
	void SetDirection(Direction direction);
	virtual void MoveWithinTheBounds(Rect bounds);
	~MainCircle();
};
