#ifndef DRIVERS_POSE_H_INCLUDED
#define DRIVERS_POSE_H_INCLUDED 1

#include "drivers.h"
#include "number/pose.h"

namespace drivers {

class Pose : public Driver
{
	public:
		Pose(msg::Channel* in_p, int in_id, msg::Channel* in_done) : Driver(in_p, in_id, in_done) {}
		virtual void main()
		{
			using namespace msg;
			Subs<num::Pose> dp  (m_p, "pose-change"); // subscribe to local to get 'true' value
			Subs<num::Pose> pose(m_p, "pose");        // subscribe to local to get 'true' value
			Subs<int>    done(m_done);

			done.value = m_myID;
			done.publish();
			
			while (true)
			{
				waitFor(dp);
				updatePose(pose.value, dp.value); // update pose
				pose.publish();               // let the others know
	
				done.value = m_myID;
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

