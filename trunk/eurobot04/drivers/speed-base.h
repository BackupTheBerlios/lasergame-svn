#ifndef DRIVERS_SPEED_H_INCLUDED
#define DRIVERS_SPEED_H_INCLUDED 1

#include "drivers.h"
#include "number/time.h"
#include "number/speed.h"

namespace drivers {

	class SpeedBase : public Driver
	{
		public:
			SpeedBase(const Params& in_params) : Driver(in_params) {}
			virtual void main()
			{
				using namespace msg;
				using namespace std;
				Subs<num::Time>  dt       (m.p, "time-change");
				Subs<num::Speed> speedReq (m.p, "speed-requested");
				Subs< std::pair<num::Time, num::Speed> > out (m.p, "time-change+speed-current");
				Subs<int> done(m.done);
				num::Speed speedCur;
				done.value = m.myID;
				done.publish();
				while (true)
				{
					msg::waitFor(dt);
					update(speedCur, speedReq.value, dt.value);
					out.value = make_pair(dt.value, speedCur);
					out.publish();
					done.value = m.myID;
					done.publish();
				}
			}
			virtual void update(num::Speed & io_speed, const num::Speed in_req, 
					const num::Time & in_dt) const = 0;
	};
	
}

#endif // DRIVERS_SPEED_H_INCLUDED

