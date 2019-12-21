#include "FieldCuttingHelper.h"

vector<FieldCuttingHelper::VerticalLine> FieldCuttingHelper::SplitIntoVerticalLines(vector<POINT> points)
{

	return vector<VerticalLine>();
}

queue<FieldCuttingHelper::VerticalLine> FieldCuttingHelper::OrderVerticalLinesByX()
{
	return queue<VerticalLine>();
}

// Сортирует точки по возрастанию координаты X алгоритмом быстрой сортировки.
void FieldCuttingHelper::QuickSortPointsByX(POINT* points, int const length)
{
	POINT pivot; // опорный  элемент
	pivot.x = 0;
	pivot.y = 0;

	if (length > 1) {
		int midIndex = length / 2;

		// Массив элементов, находящихся слева от опорного элемента (меньше опорного).
		POINT* left = new POINT[length];
		int j = 0; // индекс для прохода по элементам массива left.

		// Массив элементов, находящихся слева от опорного элемента (больше опорного).
		POINT* right = new POINT[length];
		int k = 0; // индекс для прохода по элементам массива left.

		pivot = points[midIndex];
		for (int i = 0; i < length; i++) {
			if (i != midIndex) {
				if (points[i].x < pivot.x) {
					left[j] = points[i];
					j++;
				}
				else {
					right[k] = points[i];
					k++;
				}
			}
		}

		QuickSortPointsByX(left, j);
		QuickSortPointsByX(right, k);
		for (int i = 0; i < length; i++) {
			if (i < j) {
				points[i] = left[i];;
			}
			else if (i == j) {
				points[i] = pivot;
			}
			else {
				points[i] = right[i - (j + 1)];
			}
		}
	}
}

vector<Rect> FieldCuttingHelper::SplitIntoRects(vector<POINT> points)
{
	if (points.size() % 2 != 0) {
		throw "Число точек должно быть чётным";
	}

	
	return vector<Rect>();
}
