﻿#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <queue>

using namespace std;
using namespace Gdiplus;

class FieldCuttingHelper
{
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

public:
	static queue<VerticalLine> FormVerticalLinesQueue(vector<POINT> points);
	static void QuickSortPointsByX(POINT*, int);
	static vector<Rect> SplitIntoRects(vector<POINT>);
};

