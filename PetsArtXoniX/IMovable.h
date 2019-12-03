#pragma once
#include <windows.h> // обязательно до gdiplus.h
#include <gdiplus.h>

using namespace Gdiplus;

interface IMovable
{
public:
	void MoveWithinTheBounds(Rect bounds);
};