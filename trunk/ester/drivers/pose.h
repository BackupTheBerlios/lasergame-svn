// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef DRIVERS_POSE_H_INCLUDED
#define DRIVERS_POSE_H_INCLUDED 1

#include "drivers.h"
#include "number/pose.h"
#include "../measures.h"
#include "../field.h"

namespace drivers {

class Pose : public Driver
{
	public:
		Pose(Params& in_params) : Driver(in_params) {}
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose> dp  (m.p, "pose-change");
			Subs<num::Pose> pose(m.p, "pose");
			Subs<int>       done(m.done);
			{
				thread::Locker l(m.field);
				pose.value = m.field.getRobot(m.robotID);
			}

			done.value = m.myID;
			done.publish();
			
			while (true)
			{
				waitFor(dp);
				updatePose(pose.value, dp.value); // update pose
				{
					thread::Locker l(m.field);
					m.field.setRobot(m.robotID, pose.value);
				}
				pose.publish();               // let the others know
	
				done.value = m.myID;
				done.publish();
			}
		}
		
		void updatePose(num::Pose & in_p, const num::Pose & in_dp)
		{
			in_p.advanceBy(in_dp.x());
			in_p.slideBy(in_dp.y());
			in_p.turnBy(in_dp.heading());
			in_p.heading().normalize();
		}
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

