#include "model.h"

	//{{{ TODO : unify with EsterUSB!!!
	int n = 0;
	m_floorColor[n++] = Offset(Milim(14), Milim(	96));
	m_floorColor[n++] = Offset(Milim(113), Milim(	96));
	m_floorColor[n++] = Offset(Milim(-62), Milim(	45));
	m_floorColor[n++] = Offset(Milim(15), Milim(	35));
	m_floorColor[n++] = Offset(Milim(15), Milim(	-36));
	m_floorColor[n++] = Offset(Milim(-62), Milim(	-41));
	m_floorColor[n++] = Offset(Milim(111), Milim(	-96));
	m_floorColor[n++] = Offset(Milim(14), Milim(	-96));
	ASSERT( n == m_floorColor.N_ITEM );
	//}}}}

num::Speed Model::calcSpeed(const num::Speed & in_old, const num::Time & in_dt) //{{{1
{
	int accel = 0;
	int brake = 0;
	
	if (m_currentSpeed.m_v >= num::FSpeed() && m_requestedSpeed.m_v.gt(m_currentSpeed.m_v))
		accel = 1;
	else if (m_currentSpeed.m_v <= num::FSpeed() && m_requestedSpeed.m_v.lt(m_currentSpeed.m_v))
		accel = -1;

	if (m_currentSpeed.m_v > num::FSpeed() && m_requestedSpeed.m_v.lt(m_currentSpeed.m_v))
		brake = 1;
	else if (m_currentSpeed.m_v < num::FSpeed() && m_requestedSpeed.m_v.gt(m_currentSpeed.m_v))
		brake = -1;
	
	ASSERT( !(brake != 0 && accel != 0) );

	//cout << accel << "\t" << brake << "\t" << m_requestedSpeed.m_v.mm() << "\t" 
	//	<< m_currentSpeed.m_v.mm() << endl;;

	if (accel != 0) // speed up (forward or backward)
		m_currentSpeed.m_v += (ACCEL * m_timeChange.m_dt) * accel;
	else if (brake != 0) // brake
	{
		if (m_currentSpeed.m_v * brake < num::FSpeed(Milim(60)))
		{
			m_currentSpeed.m_v -= num::FSpeed(Milim(2)) * brake;
			if (m_requestedSpeed.m_v == num::FSpeed() && m_currentSpeed.m_v * brake < num::FSpeed(Milim(5)))
			{
				//cout << ">>>>>>>>\t" << brake << "\t" << m_currentSpeed.m_v.mm() << endl;
				m_currentSpeed.m_v = num::FSpeed();
			}
		}
		else
			m_currentSpeed.m_v /= 1.0184; // exp(dt*b)==exp(0.005*3.7)==1.018672... 
	}

	if (m_currentSpeed.m_omega.gt(m_requestedSpeed.m_omega))
		m_currentSpeed.m_omega -= num::ASpeed(num::Deg(1000) * (m_timeChange.m_dt/num::Sec(1)));
	else if (m_currentSpeed.m_omega.lt(m_requestedSpeed.m_omega))
		m_currentSpeed.m_omega += num::ASpeed(num::Deg(1000) * (m_timeChange.m_dt/num::Sec(1)));
}

