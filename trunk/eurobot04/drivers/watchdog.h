#ifndef DRIVERS_WATCHDOG_H_INCLUDED
#define DRIVERS_WATCHDOG_H_INCLUDED 1

#include "drivers.h"

#include <iostream>

namespace drivers {

class Watchdog : public Driver
{
	public:
		Watchdog(Params& in_params) : Driver(in_params) {}
		virtual void main()
		{
			using namespace msg;
			using namespace std;
			Subs<bool> watchdog(m.p, "watchdog");
			Subs<int>  done    (m.done);
			done.value = m.myID;
			done.publish();
			while (true)
			{
				waitFor(watchdog);
				done.value = m.myID;
				done.publish();
			}
		}
};

}

#endif // DRIVERS_WATCHDOG_H_INCLUDED

