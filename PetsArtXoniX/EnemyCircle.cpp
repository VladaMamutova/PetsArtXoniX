#include "EnemyCircle.h"

EnemyCircle::EnemyCircle() :SimpleCircle()
{
	SetColor(COLOR);
}

EnemyCircle::EnemyCircle(int x, int y) : SimpleCircle(x, y)
{
	SetColor(COLOR);
}

EnemyCircle::EnemyCircle(EnemyCircle &circle)
{
	this->x = circle.x;
	this->y = circle.y;
	this->radius = circle.radius;
	this->color = circle.color;
}

EnemyCircle::~EnemyCircle()
{
}
