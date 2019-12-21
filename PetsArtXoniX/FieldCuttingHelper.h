#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <queue>

using namespace std;
using namespace Gdiplus;

class FieldCuttingHelper
{
public:
	struct VerticalLine
	{
		POINT top;
		POINT bottom;
	};

	struct MyRect
	{
		int top_left;
		int top_right;
		int bottom_right;
		int bottom_left;
	};

	static queue<VerticalLine> FormVerticalLinesQueue(vector<POINT> points);
	static void QuickSortPointsByX(vector<POINT>* points, int left, int right);
	static vector<Rect> SplitIntoRects(vector<POINT> points);
};

