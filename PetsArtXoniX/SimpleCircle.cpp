#include "SimpleCircle.h"

SimpleCircle::SimpleCircle()
{
	x = 0;
	y = 0;
	radius = DEFAULT_RADIUS;
	color = DEFAULT_COLOR;
}

SimpleCircle::SimpleCircle(int x, int y) {
	this->x = x;
	this->y = y;
	radius = DEFAULT_RADIUS;
	color = DEFAULT_COLOR;
}

SimpleCircle::SimpleCircle(int x, int y, int radius) {
	this->x = x;
	this->y = y;
	this->radius = radius;
	color = DEFAULT_COLOR;
}

SimpleCircle::SimpleCircle(SimpleCircle &circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
}

SimpleCircle::~SimpleCircle()
{
}

int SimpleCircle::GetX() {
	return x;
}
int SimpleCircle::GetY() {
	return y;
}
int SimpleCircle::GetRadius() {
	return radius;
}

Color SimpleCircle::GetColor() {
	return color;
}

void SimpleCircle::SetX(int x) {
	this->x = x;
}

void SimpleCircle::SetY(int y) {
	this->y = y;
}

void SimpleCircle::SetRadius(int radius) {
	this->radius = radius;
}

void SimpleCircle::SetColor(Color color) {
	this->color = color;
}

