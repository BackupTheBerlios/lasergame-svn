// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include "simul.h"
#include "measures.h" // INITIAL()
#include "util/assert.h"

#include <ctime>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace num;

namespace
{ 
	//{{{1 constants
	//const num::FBcc BRAKE(Milim(3700));
	static inline const LinearAcc    ACCEL()  { return LinearAcc(Meter(2)); }
	static inline const LinearSpeed  FSPEED() { return LinearSpeed(Meter(1)); }
	static inline const AngularSpeed ASPEED() { return AngularSpeed(Deg(180)); }
	static inline const AngularSpeed ASPEED_CORR() { return AngularSpeed(Deg(20)); }
	//! Maximum distance where GP still picks up palm
	inline const num::Dist MAX_GP_DISTANCE() { return Milim(900); }
	//! Distance that makes one unit difference on output from GP
	inline const num::Dist GP_UNIT_DIST() { return Milim(5); }
	inline const num::Dist BALL_EAT_DIST() { return Milim(60); }
	//}}}

static void updateSpeed(num::Speed & in_old, const num::Speed & in_req, const num::Time & in_dt) //{{{1
{
	using namespace num;
	int accel = 0;
	int brake = 0;
	
	if (in_old.m_linear >= LinearSpeed() && in_req.m_linear.gt(in_old.m_linear))
		accel = 1;
	else if (in_old.m_linear <= LinearSpeed() && in_req.m_linear.lt(in_old.m_linear))
		accel = -1;

	if (in_old.m_linear > LinearSpeed() && in_req.m_linear.lt(in_old.m_linear))
		brake = 1;
	else if (in_old.m_linear < LinearSpeed() && in_req.m_linear.gt(in_old.m_linear))
		brake = -1;
	
	ASSERT( !(brake != 0 && accel != 0) );

	//cout << accel << "\t" << brake << "\t" << in_req.m_linear.mm() << "\t" 
	//	<< in_old.m_linear.mm() << endl;;

	if (accel != 0) // speed up (forward or backward)
		in_old.m_linear += (ACCEL() * in_dt) * accel;
	else if (brake != 0) // brake
	{
		if (in_old.m_linear * brake < LinearSpeed(Milim(60)))
		{
			in_old.m_linear -= LinearSpeed(Milim(2)) * brake;
			if (in_req.m_linear == LinearSpeed() && in_old.m_linear * brake < LinearSpeed(Milim(5)))
			{
				//cout << ">>>>>>>>\t" << brake << "\t" << in_old.m_linear.mm() << endl;
				in_old.m_linear = LinearSpeed();
			}
		}
		else
		{
			ASSERT( in_dt == MSec(5) );
			in_old.m_linear /= 1.0184; // exp(dt*b)==exp(0.005*3.7)==1.018672... 
		}
	}

	if (in_old.m_angular.gt(in_req.m_angular))
		in_old.m_angular -= AngularSpeed(Deg(1000) * (in_dt/Sec(1)));
	else if (in_old.m_angular.lt(in_req.m_angular))
		in_old.m_angular += AngularSpeed(Deg(1000) * (in_dt/Sec(1)));
}

Pose calcPoseChange(const Speed & in_speed, const Time & in_dt) //{{{1
{
	return Pose(in_speed.m_linear * in_dt, Dist(), in_speed.m_angular * in_dt);
}

void updatePose(Pose & in_p, const Pose & in_dp, const Angle::type& AERR, const Dist::type& FERR, const Angle::type& DERR, Rnd & in_rnd) //{{{1
{
	in_p.advanceBy(in_dp.x() + in_dp.x() * FERR * in_rnd());
	in_p.turnBy(in_dp.heading() + in_dp.heading() * AERR * in_rnd());
	
	num::Meter df = (in_dp.heading()/num::Rad(1) * DERR) * in_rnd();
	num::Meter ds = (in_dp.heading()/num::Rad(1) * DERR) * in_rnd();
	in_p.offsetBy(df, ds);

	in_p.heading().normalize();
}

void updateFloorColor(FloorColor & in_floor, const Pose & in_frame, int in_side) //{{{1
{
	for (int i = 0; i < in_floor.N_ITEM; i++)
		in_floor[i].m_color = measures::getColor(in_frame.transform(in_floor[i]), in_side);
}

double calcGP2top(const Field& in_field, const Pose & in_pose) //{{{1
{
	Dist p1 = in_field.palm1Dist(in_pose.point());
	Dist p2 = in_field.palm2Dist(in_pose.point());
	Dist closer = min(p1, p2);

	if (closer < MAX_GP_DISTANCE())
		return MAX_GP_DISTANCE()/GP_UNIT_DIST() - closer/GP_UNIT_DIST();
	else
		return 0;
}

Point calcCamera(const Balls& in_balls, const Pose & in_pose) //{{{1
{
	Balls::const_iterator iter;
	Angle min = Deg(180);
	Point minOff;

	//cout << "upCamera()" << endl;
	for (iter = in_balls.begin(); iter != in_balls.end(); ++iter)
	{
		Point o = in_pose.offsetTo(*iter);
		Dist d = o;
		Angle a = o;
		if (a < Angle()) a *= -1;
		//cout << o.n() << " --- " << (*iter).n() << endl;
		//cout << setw(5) << a.rad() << " (" << (*iter).n() << ")" << endl;
		if (d > Milim(100))
		{
			if (a.lt(min, Deg(3)))
			{
				//cout << "### min:" << min.rad() << " -> " << a.rad() << ", " << d.m() << endl;
				min = a;
				minOff = o;
			}
			else if (a.lt(min+Deg(3)) && d < Dist(minOff))
			{
				//cout << "*** dist: " << Dist(minOff).m() << " -> " << d.m() << endl;
				//cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
				min = a;
				minOff = o;
			}
		}
	}
	return min < Deg(22) ? minOff : Point(Milim(-1), Dist());
	//cout << Angle(m_ballPos.m_off).deg() << endl << endl;
}
//}}}
}

Simul::Simul(msg::Channel* in_p, Field* in_field, int in_side) //{{{1
	: m_field(*in_field),
		m_watchdog    (in_p, "watchdog"),
		m_seed        (in_p, "seed"),
		m_timeChange  (in_p, "time-change"),
		m_poseChange  (in_p, "pose-change"),
		m_pose        (in_p, "true-pose"),
		m_currentSpeed(in_p, "speed-current"),
		m_reqSpeed    (in_p, "speed-requested", this, &Simul::reqSpeed),
		
		m_start       (in_p, "start"),
		m_ballPos     (in_p, "ball"),
		m_enemy       (in_p, "enemy"),
		m_floorColor  (in_p, "floor-color"),
		m_numBallsIn  (in_p, "num-balls-in"),
		m_gp2top      (in_p, "gp2top"),
		m_reqShoot    (in_p, "shoot-req", this, &Simul::reqShoot),
		
		AERR(0), FERR(0), DERR(0), m_camTick(0)
{
	ASSERT( in_field != 0 );
	//m_seed.value = time(NULL);
	m_seed.value = 1;
	m_timeChange.value = num::MSec(5);
	m_pose.value  = measures::INITIAL();
	m_start.value = true;
	m_numBallsIn.value = 0;
	m_rnd.setSeed(m_seed.value);

	// Ester
	int n = 0;
	m_floorColor.value[n++] = Point(Milim( 14), Milim( 96));
	m_floorColor.value[n++] = Point(Milim(113), Milim( 96));
	m_floorColor.value[n++] = Point(Milim(-62), Milim( 45));
	m_floorColor.value[n++] = Point(Milim( 15), Milim( 35));
	m_floorColor.value[n++] = Point(Milim( 15), Milim(-36));
	m_floorColor.value[n++] = Point(Milim(-62), Milim(-41));
	m_floorColor.value[n++] = Point(Milim(111), Milim(-96));
	m_floorColor.value[n++] = Point(Milim( 14), Milim(-96));
	ASSERT( n == m_floorColor.value.N_ITEM );
}

Simul::~Simul() //{{{1
{
}

void Simul::main() //{{{1
{
	while (true)
	{
		m_timeChange.publish();
		//cout << "Waiting for watchdog" << endl;
		waitFor(m_watchdog);
		
		updateSpeed(m_currentSpeed.value, m_reqSpeed.value, m_timeChange.value);
		m_currentSpeed.publish();
		
		m_poseChange.value = calcPoseChange(m_currentSpeed.value, m_timeChange.value);
		m_poseChange.publish();
		
		updatePose(m_pose.value, m_poseChange.value, AERR, FERR, DERR, m_rnd);
		m_pose.publish();
		
		updateFloorColor(m_floorColor.value, m_pose.value, m_side);
		m_floorColor.publish();
		
		m_gp2top.value = calcGP2top(m_field, m_pose.value);
		m_gp2top.publish();
		
		m_numBallsIn.value += m_field.tryEatBall(m_pose.value, BALL_EAT_DIST());
		m_numBallsIn.publish();

		++m_camTick %= 5;
		if (m_camTick == 0) 
		{
			m_ballPos.value = calcCamera(m_field.m_balls, m_pose.value);
			m_ballPos.publish();
		}
		
		m_enemy.value = m_pose.value.offsetTo(m_field.enemy());
		m_enemy.publish();

		m_field.checkPalms(m_pose.value.point());
		m_field.checkEnemy(m_pose.value.point());
	}
}

void Simul::reqSpeed() //{{{1
{
	//cout << "Speed received... " << m_reqSpeed.value.m_linear.mm() << endl;
}

void Simul::reqShoot() //{{{1
{
}//}}}1

#if 0
void Simul::setError(const Angle::type& in_ae, const Dist::type& in_fe, const Angle::type& in_de)//{{{1
{
	AERR = in_ae;
	FERR = in_fe;
	DERR = in_de;
}
#endif


#if 0
void EsterSimul::upBalls() //{{{1
{
	if (!m_ballIn)
		m_reqShoot.m_dist = Dist();
	else
	{
		if (m_reqShoot.m_dist.gt())
		{
			cout << "*** Shooting ball from " << "[" << m_pose.m_pose.n() << "]" << endl;

			if ((m_pose.m_pose.heading() <= Deg(-90)) || (m_pose.m_pose.heading() >= Deg(90)))
			{
			  cout << "*** MISSED: Shooting to the other side" << endl;
			}
			else
			{
				Angle ang = m_pose.m_pose.heading();
				Dist dy = (measures::SIZE_X() - m_pose.m_pose.x()) * (ang.sin()/ang.cos());
				Dist distFromCenter = m_pose.m_pose.y() + dy - measures::SIZE_Y()/2;
				Dist absDistFromCenter = (distFromCenter > 0 ? distFromCenter : -distFromCenter);
				if (absDistFromCenter < Milim(300) )
				{
			    cout << "*** SCORED HIT!!! at " << distFromCenter.mm() << " mm from center" << endl;
					m_score++;
					m_lastHitTime = m_timeChange.m_time;
				}
				else
					cout << "*** MISSED: at "<< distFromCenter.mm() << " mm from center" << endl;
			}
			cout << "*** Current score is " << m_score << ", current time is " << 
				m_timeChange.m_time.ms() / 1000.0 << " s" << endl;
			
			m_numBallsIn--;
		}
	}
}
//}}}

#endif


