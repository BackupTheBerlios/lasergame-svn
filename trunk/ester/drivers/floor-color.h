#ifndef DRIVERS_FLOOR_COLOR_H_INCLUDED
#define DRIVERS_FLOOR_COLOR_H_INCLUDED 1

#include "drivers.h"
#include "../floor-color.h"

#include <iostream>

namespace drivers {

class FloorColor : public Driver
{
	public:
		FloorColor(msg::Channel* in_p, int in_id, msg::Channel* in_done) : Driver(in_p, in_id, in_done) {}
		virtual void main()
		{
			using namespace msg;
			using namespace std;
			Subs<num::Pose> pose(m_p, "pose");
			Subs<num::FloorColor> floorColor(m_p, "floor-color");
			Subs<int> done(m_done);
			
			done.value = m_myID;
			done.publish();
			
			while (true)
			{
				//cout << "Waiting for pose..." << endl;
				waitFor(pose);
				floorColor.publish();
				
				done.value = m_myID;
				done.publish();
			}
		}
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

