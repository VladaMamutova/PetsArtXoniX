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
		bool CompareArrays(POINT p1[], POINT p2[]) {
			if (sizeof(p1) / sizeof(*p1) != (sizeof(p2) / sizeof(*p2)))
				return false;
			for (int i = 0; i < sizeof(p1) / sizeof(*p1); i++) {
				if (p1[i].x != p2[i].x && p1[i].y != p2[i].y)
					return false;
			}
			return true;
		}

		TEST_METHOD(QuickSortPointsByX1)
		{
			// Arrange
			POINT points[5];
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

			POINT expected[5];
			expected[0] = points[0];
			expected[1] = points[1];
			expected[2] = points[2];
			expected[3] = points[3];
			expected[4] = points[4];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(points, 5);

			// Assert
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(CompareArrays(expected, points));
		}

		TEST_METHOD(QuickSortPointsByX2)
		{
			// Arrange
			POINT points[4];
			points[0].x = 1;
			points[0].y = 0;

			points[1].x = 3;
			points[1].y = 0;

			points[2].x = 0;
			points[2].y = 0;

			points[3].x = 2;
			points[3].y = 0;
			
			POINT expected[4];
			expected[0] = points[2];
			expected[1] = points[0];
			expected[2] = points[1];
			expected[3] = points[3];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(points, 4);

			// Assert
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(CompareArrays(expected, points));
		}

		TEST_METHOD(QuickSortPointsByX3)
		{
			// Arrange
			POINT points[5];
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

			POINT expected[5];
			expected[0] = points[3];
			expected[1] = points[4];
			expected[2] = points[0];
			expected[3] = points[1];
			expected[4] = points[2];

			// Act
			FieldCuttingHelper::QuickSortPointsByX(points, 5);

			// Assert
			Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(CompareArrays(expected,  points));
		}
	};
}