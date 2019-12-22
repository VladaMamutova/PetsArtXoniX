#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <queue>

using namespace std;
using namespace Gdiplus;

//enum VerticalLinePositions // позиция 1-й относительно 2-й
//{
//	OnAPar, // на одном уровне (xTop1 = xTop2, xBottom1 = xBottom2)
//	Above, // 1-я над 2-й (xBottom1 = xTop2)
//	Under, // 1-я под 2-й (xTop1 = xBottom2)
//
//	Higher, // 1-я выше 2-й (xBottom1 < xTop2)
//	Below, // 1-я ниже 2-й (xTop1 > xBottom2)
//
//	Within, // 1-я помещается в пределы 2-й (xTop1 > xTop2, xBottom1 < xBottom2)
//	Beyond, // 1-я выходит за пределы 2-й (xTop1 < xTop2, xBottom1 > xBottom2)
//};

struct VerticalLine
{
	Point top;
	Point bottom;
};

struct MyRect
{
	Point top_left;
	Point top_right;
	Point bottom_right;
	Point bottom_left;

	MyRect(Point top_left, Point top_right, Point bottom_right, Point bottom_left) {
		this->top_left = top_left;
		this->top_right = top_right;
		this->bottom_right = bottom_right;
		this->bottom_left = bottom_left;
	}
};

class FieldCuttingHelper
{
public:
	static vector<Point> GetBottomConnectionPoints(Point startPoint, Point endPoint, Rect bounds);
	static void QuickSortPointsByX(vector<Point>* points, int left, int right);
	static queue<VerticalLine> FormVerticalLinesQueue(vector<Point> points);
	//static VerticalLinePositions GetVerticalLineRelativePosition(VerticalLine, VerticalLine);
	static vector<Rect> SplitIntoRects(vector<Point> points);
};

