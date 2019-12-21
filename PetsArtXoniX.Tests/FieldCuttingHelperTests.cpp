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
		bool CompareVectors(vector<POINT> p1, vector<POINT> p2) {
			if (p1.size() != p2.size())
				return false;
			for (int i = 0; i < p1.size(); i++) {
				if (p1[i].x != p2[i].x || p1[i].y != p2[i].y)
					return false;
			}
			return true;
		}

		bool CompareVerticalLineQueue(queue<FieldCuttingHelper::VerticalLine> q1,
			queue<FieldCuttingHelper::VerticalLine> q2) {
			if (q1.size() != q2.size())
				return false;
			for (size_t i = 0; i < q1.size(); i++) {
				FieldCuttingHelper::VerticalLine line1 = q1.front();
				FieldCuttingHelper::VerticalLine line2 = q2.front();
				if (line1.top.x != line2.top.x ||
					line1.top.y != line2.top.y ||
					line1.bottom.x != line2.bottom.x ||
					line1.bottom.y != line2.bottom.y)
					return false;
			}
			return true;
		}

		TEST_METHOD(QuickSortPointsByX1)
		{
			// Arrange
			vector<POINT> points(5);
			points[0].x = 0;
			points[0].y = 0;
			
			points[1].x = 1;
			points[1].y = 0;
			
			points[2].x = 2;
			points[2].y = 0;
			
			points[3].x = 3;
			points[3].y = 0;

			points[4].x = 3;
			points[4].y = 0;

			vector<POINT> expected(5);
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
			vector<POINT> points(4);
			points[0].x = 1;
			points[0].y = 0;

			points[1].x = 3;
			points[1].y = 0;

			points[2].x = 0;
			points[2].y = 0;

			points[3].x = 2;
			points[3].y = 0;
			
			vector<POINT> expected(4);
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
			vector<POINT> points(5);
			points[0].x = 4;
			points[0].y = 0;

			points[1].x = 5;
			points[1].y = 0;

			points[2].x = 5;
			points[2].y = 0;

			points[3].x = -5;
			points[3].y = 0;

			points[4].x = 0;
			points[4].y = 0;

			vector<POINT> expected(5);
			expected[0] = points[3];
			expected[1] = points[4];
			expected[2] = points[0];
			expected[3] = points[1];
			expected[4] = points[2];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(&points, 0, points.size() - 1);

			// Assert
			Assert::IsTrue(CompareVectors(expected,  points));
		}

		TEST_METHOD(FormVerticalLinesQueue1)
		{
			// Arrange
			vector<POINT> points(6);

			points[0].x = 0;
			points[0].y = 0;

			points[1].x = 0;
			points[1].y = 5;

			points[2].x = 2;
			points[2].y = 5;

			points[3].x = 2;
			points[3].y = 0;

			points[4].x = 5;
			points[4].y = 4;

			points[5].x = 5;
			points[5].y = 5;

			queue<FieldCuttingHelper::VerticalLine> expected;
			FieldCuttingHelper::VerticalLine line;

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
			queue<FieldCuttingHelper::VerticalLine> actual = 
				FieldCuttingHelper::FormVerticalLinesQueue(points);

			// Assert
			Assert::IsTrue(CompareVerticalLineQueue(expected, actual));
		}

		TEST_METHOD(FormVerticalLinesQueueThrowsSizeException)
		{
			// Arrange
			vector<POINT> points(5); // размер не кратный двум

			points[0].x = 0;
			points[0].y = 0;

			points[1].x = 0;
			points[1].y = 5;

			points[2].x = 2;
			points[2].y = 5;

			points[3].x = 2;
			points[3].y = 0;

			points[4].x = 5;
			points[4].y = 4;

			// Act
			auto func = [points] { return FieldCuttingHelper::FormVerticalLinesQueue(points); };

			// Assert
			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(FormVerticalLinesQueueThrowsNotVerticalLine)
		{
			// Arrange
			vector<POINT> points(6);

			// points[0].x != points[1].x - not vertical line
			points[0].x = 0;
			points[0].y = 0;

			points[1].x = 1;
			points[1].y = 5;

			points[2].x = 2;
			points[2].y = 5;

			points[3].x = 2;
			points[3].y = 0;

			points[4].x = 5;
			points[4].y = 4;

			points[5].x = 5;
			points[5].y = 5;

			// Act
			auto func = [points] { return FieldCuttingHelper::FormVerticalLinesQueue(points); };

			// Assert
			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(FormVerticalLinesQueueThrowsNotLine)
		{
			// Arrange
			vector<POINT> points(6);

			// points[0] = points[1] - not line
			points[0].x = 0;
			points[0].y = 0;

			points[1].x = 0; 
			points[1].y = 0;

			points[2].x = 2;
			points[2].y = 5;

			points[3].x = 2;
			points[3].y = 0;

			points[4].x = 5;
			points[4].y = 4;

			points[5].x = 5;
			points[5].y = 5;

			// Act
			auto func = [points] { return FieldCuttingHelper::FormVerticalLinesQueue(points); };

			// Assert
			Assert::ExpectException<std::invalid_argument>(func);
		}
	};
}