#ifndef DRIVERS_FLOOR_COLOR_H_INCLUDED
#define DRIVERS_FLOOR_COLOR_H_INCLUDED 1

#include "drivers.h"
#include "../floor-color.h"

#include <iostream>

namespace drivers {

class FloorColor : public Driver
{
	public:
		FloorColor(const Params& in_params) : Driver(in_params) {}
		virtual void main()
		{
			using namespace msg;
			using namespace std;
			Subs<num::Pose> pose(m.p, "pose");
			Subs<num::FloorColor> floorColor(m.p, "floor-color");
			Subs<int> done(m.done);
			
			done.value = m.myID;
			done.publish();
			
			while (true)
			{
				//cout << "Waiting for pose..." << endl;
				waitFor(pose);
				floorColor.publish();
				
				done.value = m.myID;
				done.publish();
			}
		}
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

