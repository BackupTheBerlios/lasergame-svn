#ifndef DRIVERS_SPEED_H_INCLUDED
#define DRIVERS_SPEED_H_INCLUDED 1

#include "drivers.h"
#include "number/time.h"
#include "number/speed.h"

namespace drivers {

	class SpeedBase : public Driver
	{
		public:
			SpeedBase(msg::Channel* in_p, int in_id, msg::Channel* in_done) : Driver(in_p, in_id, in_done) {}
			virtual void main()
			{
				using namespace msg;
				using namespace std;
				Subs<num::Time>  dt       (m_p, "time-change");
				Subs<num::Speed> speedReq (m_p, "speed-requested");
				Subs< std::pair<num::Time, num::Speed> > out (m_p, "time-change+speed-current");
				Subs<int> done(m_done);
				num::Speed speedCur;
				done.value = m_myID;
				done.publish();
				while (true)
				{
					msg::waitFor(dt);
					update(speedCur, speedReq.value, dt.value);
					out.value = make_pair(dt.value, speedCur);
					out.publish();
					done.value = m_myID;
					done.publish();
				}
			}
			virtual void update(num::Speed & io_speed, const num::Speed in_req, 
					const num::Time & in_dt) const = 0;
	};
	
}

#endif // DRIVERS_SPEED_H_INCLUDED

