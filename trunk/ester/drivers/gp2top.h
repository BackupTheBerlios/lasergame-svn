// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef DRIVERS_GP2TOP_H_INCLUDED
#define DRIVERS_GP2TOP_H_INCLUDED 1

#include "drivers.h"
#include "number/pose.h"

namespace drivers {

class GP2Top : public Driver
{
	public:
		GP2Top(msg::Channel* in_p, int in_id, msg::Channel* in_done) : Driver(in_p, in_id, in_done) {}
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose> pose(m_p, "pose");
			Subs<double>  gp2top(m_p, "gp2top");
			Subs<int>       done(m_done);

			done.value = m_myID;
			done.publish();

			while (true)
			{
				waitFor(pose);

				gp2top.value = 0;
				gp2top.publish();
				
				done.value = m_myID;
				done.publish();
			}
		}
#if 0
		double calcGP2top(const Field& in_field, const Pose & in_pose)
		{
			Dist p1 = in_field.palm1Dist(in_pose.point());
			Dist p2 = in_field.palm2Dist(in_pose.point());
			Dist closer = min(p1, p2);

			if (closer < MAX_GP_DISTANCE())
				return MAX_GP_DISTANCE()/GP_UNIT_DIST() - closer/GP_UNIT_DIST();
			else
				return 0;
		}
#endif
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

