#pragma once
#include "SimpleCircle.h"
#include "IMovable.h"

class EnemyCircle :public SimpleCircle, IMovable
{
private:
	const Color COLOR = Color(190, 0, 40); // красный цвет
	int deltaX;
	int deltaY;
public:
	EnemyCircle();
	EnemyCircle(int x, int y);
	EnemyCircle(int x, int y, int radius);
	EnemyCircle(const EnemyCircle&);
	EnemyCircle& operator=(const EnemyCircle&);
	void ReverseXDirection();
	void ReverseYDirection();
	virtual void MoveWithinTheBounds(Rect bounds);
	static EnemyCircle& GetRandomCircle(int radius, Rect bounds);
	~EnemyCircle();
};
