// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef DRIVERS_GP2TOP_H_INCLUDED
#define DRIVERS_GP2TOP_H_INCLUDED 1

#include "drivers.h"
#include "number/pose.h"
#include "../field.h"

namespace drivers {

class GP2Top : public Driver
{
	public:
		GP2Top(const Params& in_params) : Driver(in_params) {}
		
		//! Maximum distance where GP still picks up palm
		inline const num::Dist MAX_GP_DISTANCE() { return num::Milim(900); }
		
		//! Distance that makes one unit difference on output from GP
		inline const num::Dist GP_UNIT_DIST() { return num::Milim(5); }
		
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose> pose(m.p, "pose");
			Subs<double>  gp2top(m.p, "gp2top");
			Subs<int>       done(m.done);

			done.value = m.myID;
			done.publish();

			while (true)
			{
				waitFor(pose);

				gp2top.value = calc(m.field, pose.value);
				gp2top.publish();
				
				done.value = m.myID;
				done.publish();
			}
		}
		
		double calc(const Field& in_field, const num::Pose & in_pose)
		{
			using namespace num;
			Dist p1 = in_field.palm1Dist(in_pose.point());
			Dist p2 = in_field.palm2Dist(in_pose.point());
			Dist closer = min(p1, p2);

			if (closer < MAX_GP_DISTANCE())
				return MAX_GP_DISTANCE()/GP_UNIT_DIST() - closer/GP_UNIT_DIST();
			else
				return 0;
		}
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

