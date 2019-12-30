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
	case LOW: return "Низкая";
	case AVERAGE: return "Средняя";
	case HIGH: return "Большая";
	case VERY_HIGH: return "Высокая";
	default: return "[Unknown Speed]";
	}
}