#ifndef DRIVERS_FLOOR_COLOR_BASE_H_INCLUDED
#define DRIVERS_FLOOR_COLOR_BASE_H_INCLUDED 1

#include "drivers.h"
#include "../floor-color.h"

#include <iostream>

namespace drivers {

class FloorColorBase : public Driver
{
	public:
		FloorColorBase(const Params& in_params) : Driver(in_params) {}
		virtual void main()
		{
			using namespace msg;
			using namespace std;
			Subs<num::Pose> pose(m.p, "pose");
			Subs<num::FloorColor> floorColor(m.p, "floor-color");
			Subs<int> done(m.done);
			
			init(floorColor.value);
			done.value = m.myID;
			done.publish();
			
			while (true)
			{
				//cout << "Waiting for pose..." << endl;
				waitFor(pose);
				update(floorColor.value, pose.value);
				floorColor.publish();
				
				done.value = m.myID;
				done.publish();
			}
		}
	protected:
		virtual void init(num::FloorColor& io_floor) const = 0;
		virtual void update(num::FloorColor& io_floor, const num::Pose& in_pose) const = 0;
};

}

#endif // DRIVERS_FLOOR_COLOR_BASE_H_INCLUDED

