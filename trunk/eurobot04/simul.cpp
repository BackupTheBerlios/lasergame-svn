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
	
	if (in_old.m_forward >= LinearSpeed() && in_req.m_forward.gt(in_old.m_forward))
		accel = 1;
	else if (in_old.m_forward <= LinearSpeed() && in_req.m_forward.lt(in_old.m_forward))
		accel = -1;

	if (in_old.m_forward > LinearSpeed() && in_req.m_forward.lt(in_old.m_forward))
		brake = 1;
	else if (in_old.m_forward < LinearSpeed() && in_req.m_forward.gt(in_old.m_forward))
		brake = -1;
	
	ASSERT( !(brake != 0 && accel != 0) );

	//cout << accel << "\t" << brake << "\t" << in_req.m_forward.mm() << "\t" 
	//	<< in_old.m_forward.mm() << endl;;

	if (accel != 0) // speed up (forward or backward)
		in_old.m_forward += (ACCEL() * in_dt) * accel;
	else if (brake != 0) // brake
	{
		if (in_old.m_forward * brake < LinearSpeed(Milim(60)))
		{
			in_old.m_forward -= LinearSpeed(Milim(2)) * brake;
			if (in_req.m_forward == LinearSpeed() && in_old.m_forward * brake < LinearSpeed(Milim(5)))
			{
				//cout << ">>>>>>>>\t" << brake << "\t" << in_old.m_forward.mm() << endl;
				in_old.m_forward = LinearSpeed();
			}
		}
		else
		{
			ASSERT( in_dt == MSec(5) );
			in_old.m_forward /= 1.0184; // exp(dt*b)==exp(0.005*3.7)==1.018672... 
		}
	}

	if (in_old.m_angular.gt(in_req.m_angular))
		in_old.m_angular -= AngularSpeed(Deg(1000) * (in_dt/Sec(1)));
	else if (in_old.m_angular.lt(in_req.m_angular))
		in_old.m_angular += AngularSpeed(Deg(1000) * (in_dt/Sec(1)));
}

Pose calcPoseChange(const Speed & in_speed, const Time & in_dt) //{{{1
{
	return Pose(in_speed.m_forward * in_dt, Dist(), in_speed.m_angular * in_dt);
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
	: m_p(in_p), m_field(*in_field), AERR(0), FERR(0), DERR(0)
{
	ASSERT( in_field != 0 );
}

Simul::~Simul() //{{{1
{
}

void Simul::main() //{{{1
{
	//{{{2 Subs
	msg::Subs<int>       watchdog(m_p, "watchdog");
	msg::Subs<uint32_t>  seed(m_p, "seed");
	msg::Subs<num::Time> timeChange(m_p, "time-change");
	msg::Subs<num::Pose> poseChange(m_p, "pose-change");
	msg::Subs<num::Pose> pose(m_p, "true-pose");
	msg::Subs<num::Speed> currentSpeed(m_p, "speed-current");
	
	msg::Subs<num::Speed, Simul> reqSpeed(m_p, "speed-requested", this, &Simul::reqSpeed);
	
	msg::Subs<bool> start(m_p, "start");
	msg::Subs<num::Point> ballPos(m_p, "ball"); ///< Position of a primary ball in a local frame
	msg::Subs<num::Point> enemy(m_p, "enemy");   ///< Position of enemy in a local frame
	msg::Subs<FloorColor> floorColor(m_p, "floor-color");
	msg::Subs<int> numBallsIn(m_p, "num-balls-in");
	msg::Subs<double> gp2top(m_p, "gp2top");

	msg::Subs<num::Dist, Simul> reqShoot(m_p, "shoot-req", this, &Simul::reqShoot);

	//{{{2 Init
	//seed.value = time(NULL);
	seed.value = 1;
	num::Rnd rnd;
	rnd.setSeed(seed.value);
	int camTick = 0;
		
	timeChange.value = num::MSec(5);
	pose.value  = measures::INITIAL();
	start.value = true;
	numBallsIn.value = 0;

	// Ester
	int n = 0;
	floorColor.value[n++] = Point(Milim( 14), Milim( 96));
	floorColor.value[n++] = Point(Milim(113), Milim( 96));
	floorColor.value[n++] = Point(Milim(-62), Milim( 45));
	floorColor.value[n++] = Point(Milim( 15), Milim( 35));
	floorColor.value[n++] = Point(Milim( 15), Milim(-36));
	floorColor.value[n++] = Point(Milim(-62), Milim(-41));
	floorColor.value[n++] = Point(Milim(111), Milim(-96));
	floorColor.value[n++] = Point(Milim( 14), Milim(-96));
	ASSERT( n == floorColor.value.N_ITEM );

	//}}}

	while (true)
	{
		timeChange.publish();
		//cout << "Waiting for watchdog" << endl;
		waitFor(watchdog);
		
		updateSpeed(currentSpeed.value, reqSpeed.value, timeChange.value);
		currentSpeed.publish();
		
		poseChange.value = calcPoseChange(currentSpeed.value, timeChange.value);
		poseChange.publish();
		
		updatePose(pose.value, poseChange.value, AERR, FERR, DERR, rnd);
		pose.publish();
		
		updateFloorColor(floorColor.value, pose.value, m_side);
		floorColor.publish();
		
		gp2top.value = calcGP2top(m_field, pose.value);
		gp2top.publish();
		
		numBallsIn.value += m_field.tryEatBall(pose.value, BALL_EAT_DIST());
		numBallsIn.publish();

		++camTick %= 5;
		if (camTick == 0) 
		{
			ballPos.value = calcCamera(m_field.m_balls, pose.value);
			ballPos.publish();
		}
		
		enemy.value = pose.value.offsetTo(m_field.getRobot(!m_side).point());
		enemy.publish();

		m_field.checkPalms(pose.value.point());
		m_field.checkEnemy(pose.value.point());
	}
}

void Simul::reqSpeed() //{{{1
{
	//cout << "Speed received... " << m_reqSpeed.value.m_forward.mm() << endl;
}

void Simul::reqShoot() //{{{1
{
#if 0
	// TODO !!!
	ASSERT( m_numBallsIn.value > 0 );
	ASSERT( m_reqShoot.value.gt() );
	while (m_numBallsIn.value-- > 0)
		m_field.shootBall(m_pose.value);
#endif
}
//}}}1
#if 0
void Simul::setError(const Angle::type& in_ae, const Dist::type& in_fe, const Angle::type& in_de)//{{{1
{
	AERR = in_ae;
	FERR = in_fe;
	DERR = in_de;
}
//}}}1
#endif

