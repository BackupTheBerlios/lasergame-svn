#ifndef DRIVERS_FLOOR_COLOR_ESTER_H_INCLUDED
#define DRIVERS_FLOOR_COLOR_ESTER_H_INCLUDED 1

#include "floor-color-base.h"

#include <iostream>

namespace drivers {

class FloorColorEster : public FloorColorBase
{
	public:
		FloorColorEster(const Params& in_params) : FloorColorBase(in_params) {}
	protected:
		virtual void init(num::FloorColor& io_floor) const
		{
			using namespace num;
			int n = 0;
			io_floor[n++] = Point(Milim( 14), Milim( 96));
			io_floor[n++] = Point(Milim(113), Milim( 96));
			io_floor[n++] = Point(Milim(-62), Milim( 45));
			io_floor[n++] = Point(Milim( 15), Milim( 35));
			io_floor[n++] = Point(Milim( 15), Milim(-36));
			io_floor[n++] = Point(Milim(-62), Milim(-41));
			io_floor[n++] = Point(Milim(111), Milim(-96));
			io_floor[n++] = Point(Milim( 14), Milim(-96));
			//ASSERT( n == io_floor.N_ITEM );
		}
		virtual void update(num::FloorColor& io_floor, const num::Pose& in_frame) const
		{
			for (int i = 0; i < io_floor.N_ITEM; i++)
				io_floor[i].m_color = measures::getColor(in_frame.transform(io_floor[i]), m.robotID);
		}
};

}

#endif // DRIVERS_FLOOR_COLOR_ESTER_H_INCLUDED

