#pragma once
#include "SimpleCircle.h"

class EnemyCircle :public SimpleCircle
{
private:
	const Color COLOR = Color(230, 0, 0); // красный цвет
public:
	EnemyCircle();
	EnemyCircle(int x, int y);
	EnemyCircle(EnemyCircle&);
	~EnemyCircle();
};
