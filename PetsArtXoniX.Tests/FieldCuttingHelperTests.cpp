#include "stdafx.h"
#include "CppUnitTest.h"
#include "FieldCuttingHelper.cpp"
#include <algorithm>
#include <iterator>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PetsArtXoniXTests
{
	TEST_CLASS(FieldCuttingHelperTests)
	{
	public:
		bool CompareVectors(vector<Point> p1, vector<Point> p2) {
			if (p1.size() != p2.size())
				return false;
			for (int i = 0; i < p1.size(); i++) {
				if (p1[i].X != p2[i].X || p1[i].Y != p2[i].Y)
					return false;
			}
			return true;
		}

		bool CompareVerticalLineQueue(queue<VerticalLine> q1,
			queue<VerticalLine> q2) {
			if (q1.size() != q2.size())
				return false;
			while (!q1.empty()) {
				VerticalLine line1 = q1.front();
				VerticalLine line2 = q2.front();
				if (line1.top.X != line2.top.X ||
					line1.top.Y != line2.top.Y ||
					line1.bottom.X != line2.bottom.X ||
					line1.bottom.Y != line2.bottom.Y)
					return false;
				q1.pop();
				q2.pop();
			}
			return true;
		}

		TEST_METHOD(QuickSortPointsByX1)
		{
			// Arrange
			vector<Point> points(5);
			points[0] = Point(0, 0);
			points[1] = Point(1, 0);
			points[2] = Point(2, 0);
			points[3] = Point(3, 0);
			points[4] = Point(3, 0);

			vector<Point> expected(5);
			expected[0] = points[0];
			expected[1] = points[1];
			expected[2] = points[2];
			expected[3] = points[3];
			expected[4] = points[4];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(&points, 0, points.size() - 1);

			// Assert
			Assert::IsTrue(CompareVectors(expected, points));
		}

		TEST_METHOD(QuickSortPointsByX2)
		{
			// Arrange
			vector<Point> points(4);
			points[0] = Point(1, 0);
			points[1] = Point(3, 0);
			points[2] = Point(0, 0);
			points[3] = Point(2, 0);

			vector<Point> expected(4);
			expected[0] = points[2];
			expected[1] = points[0];
			expected[2] = points[3];
			expected[3] = points[1];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(&points, 0, points.size() - 1);

			// Assert
			Assert::IsTrue(CompareVectors(expected, points));
		}

		TEST_METHOD(QuickSortPointsByX3)
		{
			// Arrange
			vector<Point> points(5);
			points[0] = Point(4, 0);
			points[1] = Point(5, 0);
			points[2] = Point(5, 0);
			points[3] = Point(-5, 0);
			points[4] = Point(0, 0);

			vector<Point> expected(5);
			expected[0] = points[3];
			expected[1] = points[4];
			expected[2] = points[0];
			expected[3] = points[1];
			expected[4] = points[2];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(&points, 0, points.size() - 1);

			// Assert
			Assert::IsTrue(CompareVectors(expected, points));
		}

		TEST_METHOD(FormVerticalLinesQueue1)
		{
			// Arrange
			vector<Point> points(6);
			points[0] = Point(0, 0);
			points[1] = Point(0, 5);
			points[2] = Point(2, 5);
			points[3] = Point(2, 0);
			points[4] = Point(5, 4);
			points[5] = Point(5, 5);

			queue<VerticalLine> expected;
			VerticalLine line;

			line.top = points[0];
			line.bottom = points[1];
			expected.push(line);

			line.top = points[3];
			line.bottom = points[2];
			expected.push(line);

			line.top = points[4];
			line.bottom = points[5];
			expected.push(line);

			// Act
			queue<VerticalLine> actual = FieldCuttingHelper::FormVerticalLinesQueue(points);

			// Assert
			Assert::IsTrue(CompareVerticalLineQueue(expected, actual));
		}

		TEST_METHOD(FormVerticalLinesQueueThrowsSizeException)
		{
			// Arrange
			vector<Point> points(5); // размер не кратный двум
			points[0] = Point(0, 0);
			points[1] = Point(0, 5);
			points[2] = Point(2, 5);
			points[3] = Point(2, 0);
			points[4] = Point(5, 4);

			// Act
			auto func = [points] { return FieldCuttingHelper::FormVerticalLinesQueue(points); };

			// Assert
			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(FormVerticalLinesQueueThrowsNotVerticalLine)
		{
			// Arrange
			vector<Point> points(6);

			// points[0].x != points[1].x - not vertical line
			points[0] = Point(0, 0);
			points[1] = Point(1, 5);
			points[2] = Point(2, 5);
			points[3] = Point(2, 0);
			points[4] = Point(5, 4);
			points[5] = Point(5, 5);

			// Act
			auto func = [points] { return FieldCuttingHelper::FormVerticalLinesQueue(points); };

			// Assert
			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(FormVerticalLinesQueueThrowsNotLine)
		{
			// Arrange
			vector<Point> points(6);

			// points[0] = points[1] - not line
			points[0] = Point(0, 0);
			points[1] = Point(0, 0);
			points[2] = Point(2, 5);
			points[3] = Point(2, 0);
			points[4] = Point(5, 4);
			points[5] = Point(5, 5);

			// Act
			auto func = [points] { return FieldCuttingHelper::FormVerticalLinesQueue(points); };

			// Assert
			Assert::ExpectException<std::invalid_argument>(func);
		}
	};
}