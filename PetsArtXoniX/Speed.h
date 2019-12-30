#pragma once

enum SPEED {
	LOW = 0,
	AVERAGE = 1,
	HIGH = 2,
	VERY_HIGH = 3
};

inline const char* ToString(SPEED speed)
{
	switch (speed)
	{
	case LOW: return "низкая";
	case AVERAGE: return "средняя";
	case HIGH: return "большая";
	case VERY_HIGH: return "высокая";
	default: return "[Unknown Speed]";
	}
}