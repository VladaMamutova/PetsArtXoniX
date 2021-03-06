﻿#include "SimpleCircle.h"

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

SimpleCircle::SimpleCircle(const SimpleCircle &circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
}

SimpleCircle & SimpleCircle::operator=(const SimpleCircle & circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
	return *this;
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

Gdiplus::Color SimpleCircle::GetColor() {
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

void SimpleCircle::SetColor(Gdiplus::Color color) {
	this->color = color;
}

Gdiplus::Point SimpleCircle::GetPosition()
{
	return Gdiplus::Point(x, y);
}

void SimpleCircle::SetPosition(Gdiplus::Point position)
{
	x = position.X;
	y = position.Y;
}