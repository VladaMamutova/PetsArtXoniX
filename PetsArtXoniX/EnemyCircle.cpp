#include "EnemyCircle.h"
//#include <time.h>
#define Gdiplus
EnemyCircle::EnemyCircle() :SimpleCircle()
{
	SetColor(COLOR);
}

EnemyCircle::EnemyCircle(int x, int y) : SimpleCircle(x, y)
{
	SetColor(COLOR);
	deltaX = 0;
	deltaY = 0;
}

EnemyCircle::EnemyCircle(int x, int y, int radius) : SimpleCircle(x, y, radius)
{
	SetColor(COLOR);
	deltaX = 0;
	deltaY = 0;
}

EnemyCircle::EnemyCircle(const EnemyCircle &circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
	this->deltaX = circle.deltaX;
	this->deltaY = circle.deltaY;
}

EnemyCircle & EnemyCircle::operator=(const EnemyCircle & circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
	this->deltaX = circle.deltaX;
	this->deltaY = circle.deltaY;
	return *this;
}

EnemyCircle::~EnemyCircle()
{
}

void EnemyCircle::MoveWithinTheBounds(Rect bounds) {
	x += deltaX;
	if (x <= bounds.GetLeft()) {
		deltaX = -deltaX;
		x += deltaX;
	}
	else if (x >= bounds.Width - 1) {
		deltaX = -deltaX;
		x += deltaX;
	}

	y += deltaY;
	if (y <= bounds.GetTop()) {
		deltaY = -deltaY;
		y += deltaY;
	}
	else if (y >= bounds.Height - 1) {
		deltaY = -deltaY;
		y += deltaY;
	}
}

void EnemyCircle::ReverseXDirection()
{
	deltaX = -deltaX;
}

void EnemyCircle::ReverseYDirection()
{
	deltaY = -deltaY;
}

EnemyCircle & EnemyCircle::GetRandomCircle(int radius, Rect bounds)
{
	EnemyCircle *circle = new EnemyCircle();
	circle->radius = radius;

	circle->x = bounds.GetLeft() + rand() % bounds.Width;
	circle->y = bounds.GetTop() + rand() % bounds.Height;

	// Генерируем направление движения.
	circle->deltaX = rand() % 2; // 0 или 1
	circle->deltaY = rand() % 2; // 0 или 1

	if (circle->deltaX == 0) {
		circle->deltaX = -1; // deltaX = 0 или 1
	}

	if (circle->deltaY == 0) {
		circle->deltaY = -1; // deltaY = 0 или 1
	}

	return *circle;
}
