#include "MainCircle.h"

MainCircle::MainCircle() :SimpleCircle()
{
	SetColor(COLOR);
}

MainCircle::MainCircle(int x, int y) : SimpleCircle(x, y)
{
	SetColor(COLOR);
}

MainCircle::MainCircle(int x, int y, int radius) : SimpleCircle(x, y, radius)
{
	SetColor(COLOR);
}

MainCircle::MainCircle(const MainCircle &circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
}

MainCircle & MainCircle::operator=(const MainCircle & circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;

	return *this;
}

MainCircle::~MainCircle()
{
}

void MainCircle::SetDirection(Direction direction) {
	this->direction = direction;
}

Direction MainCircle::GetDirection()
{
	return direction;
}

void MainCircle::MoveWithinTheBounds(Rect bounds) {
	if (direction == Direction::None) {
		return;
	}

	int xDelta = 0;
	int yDelta = 0;

	switch (direction)
	{
	case Up: {
		yDelta--;
		break;
	}
	case Right: {
		xDelta++;
		break;
	}
	case Down: {
		yDelta++;
		break;
	}
	case Left: {
		xDelta--;
		break;
	}
	default:
		break;
	}

	// Проверяем, не будет ли выходить шарик выходить за границы области.
	// Если достиг границы, шарик останавливается.

	if (x + xDelta < bounds.GetLeft()) {
		x = bounds.GetLeft();
		direction = Direction::None;
	}
	else if (x + xDelta > bounds.Width) {
		x = bounds.Width;
		direction = Direction::None;
	}
	else {
		x += xDelta;
	}

	if (y + yDelta  < bounds.GetTop()) {
		y = bounds.GetTop();
		direction = Direction::None;
	}
	else if (y + yDelta >= bounds.Height) {
		y = bounds.Height - 1;
		direction = Direction::None;
	}
	else {
		y += yDelta;
	}
}