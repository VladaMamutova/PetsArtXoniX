#include "MainCircle.h"

MainCircle::MainCircle() :SimpleCircle()
{
	SetColor(COLOR);
}

MainCircle::MainCircle(int x, int y) : SimpleCircle(x, y)
{
	SetColor(COLOR);
}

MainCircle::MainCircle(MainCircle &circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
}

MainCircle::~MainCircle()
{
}
