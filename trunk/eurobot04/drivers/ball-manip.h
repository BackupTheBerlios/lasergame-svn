#ifndef DRIVERS_BALLS_H_INCLUDED
#define DRIVERS_BALLS_H_INCLUDED 1

#include "drivers.h"
#include "util/thread.h"
#include "../field.h"

namespace drivers {

class BallManip : public Driver
{
	msg::Subs<int>     m_inBalls;
	public:
		BallManip(const Params& in_params) : Driver(in_params) {}
		inline const num::Dist BALL_EAT_DIST() { return num::Milim(60); }
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose>  pose(m.p, "pose");
			m_inBalls.subscribe(m.p, "in-balls");
			Subs<num::Dist, BallManip> shoot(m.p, "shoot", this, &BallManip::shoot);
			Subs<int>        done(m.done);
			
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
				m_inBalls.value += ballsEaten;
				//if (ballsEaten)
					m_inBalls.publish();
				
				done.value = m.myID;
				done.publish();
			}
		}
	private:
		void shoot(num::Dist& in_dist)
		{
			thread::Locker l(m.field);
			while (m_inBalls.value-- > 0)
				m.field.shootBall(m.field.getRobot(m.robotID));
		}
};

}

#endif // DRIVERS_SPEED_H_INCLUDED

