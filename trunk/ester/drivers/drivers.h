#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED 1

#include "util/msg.h"

namespace drivers {

	typedef msg::Runnable* (*pFac)(msg::Channel* in_p, int in_id, msg::Channel* in_done);

	class Driver : public msg::Runnable
	{
		protected:
			msg::Channel* m_p;
			const int m_myID;
			msg::Channel* m_done;
			Driver(msg::Channel* in_p, int in_id, msg::Channel* in_done) 
				: m_p(in_p), m_myID(in_id), m_done(in_done) {}
	};
	
	pFac getFactory(const char* in_name);
}

#endif // DRIVER_H_INCLUDED

