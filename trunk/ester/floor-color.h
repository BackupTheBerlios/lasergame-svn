#ifndef FLOOR_COLOR_H_INCLUDED
#define FLOOR_COLOR_H_INCLUDED 1

#include "number/pose.h"
#include "measures.h"

class FloorColor
{
public:
	enum { N_ITEM = 8 };
	struct Item : public num::Point
	{
		Item() : m_color(measures::WHITE) {}
		Item& operator = (const num::Point& off) { *((num::Point*)this) = off; return *this; }
		measures::Color m_color;
	}	m_items[N_ITEM];
};

#endif // FLOOR_COLOR_H_INCLUDED

