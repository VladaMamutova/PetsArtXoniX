#include "FieldCuttingHelper.h"

// Формирует очередь вертикальных линий последовательно из каждой пары точек.
queue<FieldCuttingHelper::VerticalLine> FieldCuttingHelper::
FormVerticalLinesQueue(vector<POINT> points)
{
	if (points.size() % 2 != 0) {
		throw std::invalid_argument("Чтобы сформировать вертикальные линии, "
			"число точек должно быть чётным.");
	}

	queue<VerticalLine> queue;
	VerticalLine line;
	char errorMessage[100];

	for (size_t i = 0; i < points.size(); i += 2) {
		if (points[i].x != points[i + 1].x) {
			sprintf_s(errorMessage, "Из точки (%d; %d) и точки (%d; %d) нельзя "
				"сформировать вертикальную линию. Коорднаты X не совпадают.",
				points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
			throw std::invalid_argument(errorMessage);
		}
		else if (points[i].y == points[i + 1].y) {
			sprintf_s(errorMessage, "Из точки (%d; %d) и точки (%d; %d) нельзя "
				"сформировать вертикальную линию. Точки совпадают.",
				points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
			throw std::invalid_argument(errorMessage);
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
void FieldCuttingHelper::QuickSortPointsByX(vector<POINT> *points, int left, int right)
{	
	// 1. Выбор опорного элемента (центральный).
	POINT pivot = points->at((left + right) / 2); // опорный  элемент
	   
	// 2. Разделение массива - перераспределение элементов в массиве таким образом,
	// что элементы меньше опорного помещаются перед ним, а больше или равные - после.
   	int i = left, j = right;
	while (i <= j) {
		// В левой части массива оставляем элементы, которые меньше опорного.
		while (points->at(i).x < pivot.x) {
			i++;
		}

		// В правой части оставляем элементы, которые больше опорного.
		while (points->at(j).x > pivot.x) {
			j--;
		}

		if (i <= j) {
			POINT temp;
			temp = points->at(i);
			points->at(i) = points->at(j);
			points->at(j) = temp;
			i++;
			j--;
		}
	};

	// 3. Рекурсия - рекурсивно выполняем первые два шага
	// к двум подмассивам слева и справа от опорного элемента..

	if (left < j) {
		QuickSortPointsByX(points, left, j);
	}

	if (i < right) {
		QuickSortPointsByX(points, i, right);
	}
}

// Разбивает исходную сложную фигуру, представленную в виде массива
// вершин, по прямоугольникам. 
vector<Rect> FieldCuttingHelper::SplitIntoRects(vector<POINT> points)
{
	if (points.size() < 4) {
		throw "Точки не формируют фигуру из прямоугольников.";
	}

	if (points.size() % 2 != 0) {
		throw "Число точек должно быть чётным";
	}

	QuickSortPointsByX(&points, 0, points.size() - 1);
	queue<VerticalLine> lines = FormVerticalLinesQueue(points);

	return vector<Rect>();
}
