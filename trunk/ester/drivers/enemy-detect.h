// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef DRIVERS_ENEMY_DETECT_H_INCLUDED
#define DRIVERS_ENEMY_DETECT_H_INCLUDED 1

#include "drivers.h"
#include "number/pose.h"

namespace drivers {

class EnemyDetect : public Driver
{
	public:
		EnemyDetect(const Params& in_params) : Driver(in_params) {}
		virtual void main()
		{
			msg::Subs<num::Pose> pose(m.p, "pose");
			msg::Subs<num::Point> enemy(m.p, "enemy-offset");
			msg::Subs<int> done(m.done);
			done.value = m.myID;
			done.publish();
			
			while (true)
			{
				waitFor(pose);
				{
					thread::Locker l(m.field);
					num::Pose e = m.field.getRobot(!m.robotID);
					e.x() = measures::SIZE_X() - e.x();
					e.y() = measures::SIZE_Y() - e.y();
					enemy.value = pose.value.offsetTo(e.point());
				}
				enemy.publish();
			}
		}
};

}

#endif // DRIVERS_ENEMY_DETECT_H_INCLUDED

