#ifndef DRIVERS_WATCHDOG_H_INCLUDED
#define DRIVERS_WATCHDOG_H_INCLUDED 1

#include "drivers.h"

#include <iostream>

namespace drivers {

class Watchdog : public Driver
{
	public:
		Watchdog(msg::Channel* in_p, int in_id, msg::Channel* in_done) : Driver(in_p, in_id, in_done)
		{
		}
		virtual void main()
		{
			using namespace msg;
			using namespace std;
			Subs<bool> watchdog(m_p, "watchdog");
			Subs<int>  done    (m_done);
			done.value = m_myID;
			done.publish();
			while (true)
			{
				waitFor(watchdog);
				done.value = m_myID;
				done.publish();
			}
		}
};

}

#endif // DRIVERS_WATCHDOG_H_INCLUDED

