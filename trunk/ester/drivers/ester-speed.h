#ifndef DRIVERS_ESTER_SPEED_H_INCLUDED
#define DRIVERS_ESTER_SPEED_H_INCLUDED 1

#include "drivers.h"
#include "speed-base.h"
#include "util/assert.h"

namespace drivers {
	namespace ester {

	class Speed : public drivers::SpeedBase
	{
		public:
			Speed(msg::Channel* in_p, int in_id, msg::Channel* in_done) : SpeedBase(in_p, in_id, in_done) 
			{
				//ASSERT( false );
			}
			
			static inline const num::LinearAcc ACCEL() 
			{ 
				return num::LinearAcc(num::Meter(2)); 
			}
			
			virtual void update(num::Speed & io_speed, const num::Speed in_req, const num::Time & in_dt) const
			{
				using namespace num;
				int accel = 0;
				int brake = 0;
				
				if (io_speed.m_forward >= LinearSpeed() && in_req.m_forward.gt(io_speed.m_forward))
					accel = 1;
				else if (io_speed.m_forward <= LinearSpeed() && in_req.m_forward.lt(io_speed.m_forward))
					accel = -1;
			
				if (io_speed.m_forward > LinearSpeed() && in_req.m_forward.lt(io_speed.m_forward))
					brake = 1;
				else if (io_speed.m_forward < LinearSpeed() && in_req.m_forward.gt(io_speed.m_forward))
					brake = -1;
				
				ASSERT( !(brake != 0 && accel != 0) );
			
				//cout << accel << "\t" << brake << "\t" << in_req.m_forward.mm() << "\t" 
				//	<< io_speed.m_forward.mm() << endl;;
			
				if (accel != 0) // speed up (forward or backward)
					io_speed.m_forward += (ACCEL() * in_dt) * accel;
				else if (brake != 0) // brake
				{
					if (io_speed.m_forward * brake < LinearSpeed(Milim(60)))
					{
						io_speed.m_forward -= LinearSpeed(Milim(2)) * brake;
						if (in_req.m_forward == LinearSpeed() && io_speed.m_forward * brake < LinearSpeed(Milim(5)))
						{
							//cout << ">>>>>>>>\t" << brake << "\t" << io_speed.m_forward.mm() << endl;
							io_speed.m_forward = LinearSpeed();
						}
					}
					else
					{
						ASSERT( in_dt == MSec(5) );
						io_speed.m_forward /= 1.0184; // exp(dt*b)==exp(0.005*3.7)==1.018672... 
					}
				}
			
				if (io_speed.m_angular.gt(in_req.m_angular))
					io_speed.m_angular -= AngularSpeed(Deg(1000) * (in_dt/Sec(1)));
				else if (io_speed.m_angular.lt(in_req.m_angular))
					io_speed.m_angular += AngularSpeed(Deg(1000) * (in_dt/Sec(1)));
			}
	};
	
}
}

#endif // DRIVERS_SPEED_H_INCLUDED


