#include "FieldCuttingHelper.h"

// Формирует очередь вертикальных линий последовательно из каждой пары точек.
queue<FieldCuttingHelper::VerticalLine> FieldCuttingHelper::
FormVerticalLinesQueue(vector<POINT> points)
{
	if (points.size() % 2 != 0) {
		throw "Чтобы сформировать вертикальные линии, "
			"число точек должно быть чётным.";
	}

	queue<VerticalLine> queue;
	VerticalLine line;
	char errorMessage[100];

	for (size_t i = 0; i < points.size(); i += 2) {
		if (points[i].x != points[i + 1].x) {
			sprintf_s(errorMessage, "Из точки (%d; %d) и точки (%d; %d) нельзя "
				"сформировать вертикальную линию. Коорднаты X не совпадают.",
				points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
			throw errorMessage;
		}
		else if (points[i].y == points[i + 1].y) {
			sprintf_s(errorMessage, "Из точки (%d; %d) и точки (%d; %d) нельзя "
				"сформировать вертикальную линию. Точки совпадают.",
				points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
			throw errorMessage;
		}
		else {
			if (points[i].y <= points[i + 1].y) {
				// Начало координат - верхний левый угол, поэтому верхняя точка линии
				// будет с меньшим значенией координаты Y.
				line.top = points[i];
				line.bottom = points[i + 1];
			}
			else {
				line.top = points[i + 1];
				line.bottom = points[i];
			}

			queue.push(line);
		}
	}

	return queue;
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
