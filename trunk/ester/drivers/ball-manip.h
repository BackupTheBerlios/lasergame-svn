#ifndef DRIVERS_BALLS_H_INCLUDED
#define DRIVERS_BALLS_H_INCLUDED 1

#include "drivers.h"
#include "util/thread.h"

namespace drivers {

class BallManip : public Driver
{
	public:
		BallManip(const Params& in_params) : Driver(in_params) {}
		inline const num::Dist BALL_EAT_DIST() { return num::Milim(60); }
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose> pose(m.p, "pose");
			Subs<int>    inBalls(m.p, "in-balls");
			Subs<int>       done(m.done);
			
			done.value = m.myID;
			done.publish();
			
			int ballsEaten = 0;
			while (true)
			{
				waitFor(pose);
				{
					thread::Locker l(m.field);
					ballsEaten = m.field.tryEatBall(pose.value, BALL_EAT_DIST());
				}
				inBalls.value += ballsEaten;
				//if (ballsEaten)
					inBalls.publish();
				
				done.value = m.myID;
				done.publish();
			}
		}
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

