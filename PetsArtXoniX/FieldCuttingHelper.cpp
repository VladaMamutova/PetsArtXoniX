#include "FieldCuttingHelper.h"

vector<Point> FieldCuttingHelper::GetMinConnectingPoints(Point startPoint, Point endPoint, Rect bounds) {
	vector<Point> points;
	if (startPoint.X == endPoint.X || startPoint.Y == endPoint.Y) {
		return points;
	}
	
	Point leftTop = Point(bounds.X, bounds.Y);
	Point rightTop = Point(bounds.X + bounds.Width, bounds.Y);
	Point leftBottom = Point(bounds.X, bounds.Y + bounds.Height);
	Point rightBottom = Point(bounds.X + bounds.Width, bounds.Y + bounds.Height);
	
	if (startPoint.X == bounds.X) {
		points.push_back(leftBottom);
		if (leftBottom.X == endPoint.X || leftBottom.Y == endPoint.Y) {
			return points;
		}
		points.push_back(rightBottom);
		if (rightBottom.X == endPoint.X || rightBottom.Y == endPoint.Y) {
			return points;
		}
		else {
			points.clear();
			points.push_back(leftTop);
			return points;
		}
	}

	if (startPoint.X == bounds.X + bounds.Width) {
		points.push_back(rightBottom);
		if (rightBottom.X == endPoint.X || rightBottom.Y == endPoint.Y) {
			return points;
		}
		points.push_back(leftBottom);
		if (leftBottom.X == endPoint.X || leftBottom.Y == endPoint.Y) {
			return points;
		}
		else {
			points.clear();
			points.push_back(rightTop);
			return points;
		}
	}

	if (startPoint.Y == bounds.Y) {
		points.push_back(leftTop);
		if (leftTop.X == endPoint.X || leftTop.Y == endPoint.Y) {
			return points;
		}
		points.push_back(leftBottom);
		if (leftBottom.X == endPoint.X || leftBottom.Y == endPoint.Y) {
			return points;
		}
		else {
			points.clear();
			points.push_back(rightTop);
			return points;
		}
	}

	if (startPoint.Y == bounds.Y + bounds.Height) {
		points.push_back(leftBottom);
		if (leftBottom.X == endPoint.X || leftBottom.Y == endPoint.Y) {
			return points;
		}
		points.push_back(leftTop);
		if (leftTop.X == endPoint.X || leftTop.Y == endPoint.Y) {
			return points;
		}
		else {
			points.clear();
			points.push_back(rightBottom);
			return points;
		}
	}

	return points;
}

// Сортирует точки по возрастанию координаты X алгоритмом быстрой сортировки.
void FieldCuttingHelper::QuickSortPointsByX(vector<Point> *points, int left, int right)
{
	// 1. Выбор опорного элемента (центральный).
	Point pivot = points->at((left + right) / 2); // опорный  элемент

	// 2. Разделение массива - перераспределение элементов в массиве таким образом,
	// что элементы меньше опорного помещаются перед ним, а больше или равные - после.
	int i = left, j = right;
	while (i <= j) {
		// В левой части массива оставляем элементы, которые меньше опорного.
		while (points->at(i).X < pivot.X) {
			i++;
		}

		// В правой части оставляем элементы, которые больше опорного.
		while (points->at(j).X > pivot.X) {
			j--;
		}

		if (i <= j) {
			Point temp;
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


// Формирует очередь вертикальных линий последовательно из каждой пары точек.
queue<VerticalLine> FieldCuttingHelper::FormVerticalLinesQueue(vector<Point> points)
{
	if (points.size() % 2 != 0) {
		throw std::invalid_argument("Чтобы сформировать вертикальные линии, "
			"число точек должно быть чётным.");
	}

	queue<VerticalLine> queue;
	VerticalLine line;
	char errorMessage[100];

	for (size_t i = 0; i < points.size(); i += 2) {
		if (points[i].X != points[i + 1].X) {
			sprintf_s(errorMessage, "Из точки (%d; %d) и точки (%d; %d) нельзя "
				"сформировать вертикальную линию. Коорднаты X не совпадают.",
				points[i].X, points[i].Y, points[i + 1].X, points[i + 1].Y);
			throw std::invalid_argument(errorMessage);
		}
		else if (points[i].Y == points[i + 1].Y) {
			sprintf_s(errorMessage, "Из точки (%d; %d) и точки (%d; %d) нельзя "
				"сформировать вертикальную линию. Точки совпадают.",
				points[i].X, points[i].Y, points[i + 1].X, points[i + 1].Y);
			throw std::invalid_argument(errorMessage);
		}
		else {
			if (points[i].Y <= points[i + 1].Y) {
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

// Разбивает сложную замкнутую фигуру, представленную в виде массива
// вершин, по прямоугольникам. 
vector<Rect> FieldCuttingHelper::SplitIntoRects(vector<Point> points)
{
	if (points.size() < 4) {
		throw std::invalid_argument("Точки не формируют фигуру из прямоугольников.");
	}

	if (points.size() % 2 != 0) {
		throw std::invalid_argument("Число точек должно быть чётным");
	}

	QuickSortPointsByX(&points, 0, points.size() - 1);
	queue<VerticalLine> lines = FormVerticalLinesQueue(points);

	// Массив сформированных прямоугольников из точек.
	vector<Rect> rects;
	VerticalLine line1;
	VerticalLine line2;
	Point line2Top;
	Point line2Bottom;
	while (lines.size() > 1) {
		// Первая в очереди вертикальная линия -
		// правая сторона нового прямоугольника.
		line1 = lines.front();
		lines.pop();
		line2 = lines.front();

		if (line1.top.Y == line2.top.Y) { // top совпадают
			line2.bottom.Y = line1.bottom.Y;

			lines.front().top = lines.front().bottom;
			lines.front().bottom = line2.bottom;
		}
		else if (line1.bottom.Y == line2.bottom.Y) { // bottom совпадают
			line2.top.Y = line1.top.Y;

			lines.front().bottom = lines.front().top;
			lines.front().top = line2.top;
		}
		else if (line1.top.Y == line2.bottom.Y) { // top совпадает c bottom
			line2.top.Y = line1.top.Y;
			line2.bottom.Y = line1.bottom.Y;

			lines.front().bottom = line2.bottom;
		}
		else if (line1.bottom.Y == line2.top.Y) { // bottom совпадает c top
			line2.top.Y = line1.top.Y;
			line2.bottom.Y = line1.bottom.Y;

			lines.front().top = line2.top;
		}

		rects.push_back(Rect(line1.top.X, line1.top.Y, line2.top.X - line1.top.X, line2.bottom.Y - line2.top.Y));//x,y,w,h
		//switch (GetVerticalLineRelativePosition(line1, line2)) {
		//case OnAPar: {
		//	
		//	break;
		//}
		//case Above: {
		//	line2Top = Point(line2.top.X, line1.top.Y);
		//	line2Bottom = line2.top;
		//	//rects.push_back(MyRect(line1.top, line2.top, line2.bottom, line1.bottom));
		//	break;
		//}
		//case Under: {
		//	line2Top = Point(line2.top.X, line1.top.Y);
		//	line2Bottom = line2.top;
		//	line1.bottom.X = line2.top.X;
		//	line1.bottom.Y = line1.top.Y;
		//	rects.push_back(MyRect(line1.top, line2.top, line2.bottom, line1.bottom));
		//	break;
		//}
		//case Higher: {
		//	//rects.push_back(MyRect(line1.top, line2.top, line2.bottom, line1.bottom));
		//	break;
		//}
		//case Below: {
		//	//rects.push_back(MyRect(line1.top, line2.top, line2.bottom, line1.bottom));
		//	break;
		//}
		//case Within: {
		//	break;
		//}
		//case Beyond: {
		//	break;
		//}
		//default: {
		//	break;
		//}
		//}

		//rects.push_back(MyRect(line1.top, line2.top, line2.bottom, line1.bottom));
	}

	return rects;
}

//VerticalLinePositions FieldCuttingHelper::GetVerticalLineRelativePosition(VerticalLine line1, VerticalLine line2) {
//	if (line1.top.X == line2.top.X && line1.bottom.X == line2.bottom.X)
//		return VerticalLinePositions::OnAPar;
//
//	if (line1.bottom.X == line2.top.X)
//		return VerticalLinePositions::Above;
//
//	if (line1.top.X == line2.bottom.X)
//		return VerticalLinePositions::Under;
//
//	if (line1.bottom.X < line2.top.X)
//		return VerticalLinePositions::Higher;
//
//	if (line1.top.X > line2.bottom.X)
//		return VerticalLinePositions::Below;
//
//	if (line1.top.X > line2.top.X && line1.bottom.X < line2.bottom.X)
//		return VerticalLinePositions::Within;
//
//	if (line1.top.X < line2.top.X && line1.bottom.X > line2.bottom.X)
//		return VerticalLinePositions::Beyond;
//}
