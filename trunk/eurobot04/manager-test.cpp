// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include <iostream>
using namespace std;

#include "manager.h"
#include "floor-color.h"
#include "number/speed.h"
#include "util/conf.h"
#include "util/assert.h"
#include "util/unit-test.h"
using namespace num;
using namespace msg;

namespace {

AUTOTEST(testTime) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time> dt(dirA, "time-change");

	conf::Robot a("watchdog"); conf::Robot b; Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	int i = 0;
	for ( ; i < 10; i++)
	{
		waitFor(dt);
		REQUIRE( dt.value == MSec(5) );
		watchdog.publish();
	}
	REQUIRE( i == 10 );
}

AUTOTEST(testLinearSpeed) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs< std::pair<num::Time, num::Speed> > cur(dirA, "time-change+speed-current");
	Subs<Speed> req(dirA, "speed-requested");
	
	conf::Robot a("watchdog, ester-speed");
	conf::Robot b;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(cur);
	int i = 0;

	// accelerate
	Time t = MSec(500);
	req.value.m_forward = Milim(300);
	//cout << req.value.m_forward.mm() << endl; 
	req.publish();
	while( t.gt() && cur.value.second.m_forward.lt(LinearSpeed(Milim(250))) )
	{
		t -= cur.value.first;
		i++;
		//cout << t.ms() << ": " << cur.value.second.m_forward.mm() << endl;
		watchdog.publish();
		waitFor(cur);
	}
	//cout << endl;
	REQUIRE( !cur.value.second.m_forward.lt(LinearSpeed(Milim(250))) );

	// decelerate
	t = MSec(600);
	req.value.m_forward = Milim(0);
	req.publish();
	while( t.gt() && cur.value.second.m_forward.gt() )
	{
		t -= cur.value.first;
		i++;
		//cout << t.ms() << ": " << currentSpeed.m_v.mm() << endl;
		watchdog.publish();
		waitFor(cur);
	}
	REQUIRE( !cur.value.second.m_forward.gt() );
	//cout << i;
	REQUIRE( i == 132); // 136
}

AUTOTEST(testMove) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time> dt(dirA, "time-change");
	Subs<Pose> dp(dirA, "pose-change");
	Subs<Speed> req(dirA, "speed-requested");
	
	conf::Robot a("watchdog, ester-speed, pose-change");
	conf::Robot b;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(dp);
	
	int i = 0;
	
	Time t = Sec(2);
	Meter d(1);
	req.value.m_forward = Milim(1070);
	req.publish();
	while( t.gt() && d.gt() )
	{
		t -= dt.value;
		d -= dp.value.x();
		i++;
		watchdog.publish();
		waitFor(dp);
		//cout << t.ms() << ": " << d.mm() << endl;
	}
	REQUIRE( !d.gt() );
	REQUIRE( i == 241 );
}

AUTOTEST(testTurn) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time> dt(dirA, "time-change");
	Subs<Pose> dp(dirA, "pose-change");
	Subs<Speed> req(dirA, "speed-requested");
	
	conf::Robot a("watchdog, ester-speed, pose-change");
	conf::Robot b;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(dp);
	int i = 0;
	
	Time t = Sec(2);
	Deg aa(180);
	req.value.m_angular = AngularSpeed(Deg(180));
	req.publish();
	while( t.gt() && aa.gt() )
	{
		t -= dt.value;
		aa -= dp.value.heading();
		i++;
		//cout << t.ms() << ": " << a.deg() << endl;
		watchdog.publish();
		waitFor(dp);
	}
	REQUIRE( aa.lt() );
	REQUIRE( i == 217 );
}

AUTOTEST(testTruePose) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time>   dt(dirA, "time-change");
	Subs<Speed> req(dirA, "speed-requested");
	Subs<Pose> pose(dirA, "pose");
	
	conf::Robot a("watchdog, ester-speed, pose-change, pose");
	conf::Robot b;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(pose);

	Time t = Sec(1);
	req.value.m_forward = Meter(1);
	req.publish();
	while( t.gt() )
	{
		t -= dt.value;
		//cout << t.ms() << ": " << pose.value << endl;
		watchdog.publish();  // step forward
		waitFor(pose);
	}
	REQUIRE( pose.value.x() > Milim(500) );
}

AUTOTEST(testFloorColor) //{{{1
{ 
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<FloorColor> floorColor(dirA, "floor-color");
	
	conf::Robot a("watchdog, ester-speed, pose-change, pose, floor-color");
	conf::Robot b;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(floorColor); // wait until simulator is ready
	watchdog.publish();  // make step
	waitFor(floorColor); // wait for floor color message to come in
	
	// TODO how to test this further?
}

AUTOTEST(testGP2) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time> dt(dirA, "time-change");
	Subs<Speed> req(dirA, "speed-requested");
	Subs<Pose> pose(dirA, "pose");
	Subs<double> gp2(dirA, "gp2top");

	conf::Robot a("watchdog, ester-speed, pose-change, pose, gp2top");
	conf::Robot b;
	Field field;
	field.setPalm(3,5);
	Task e(new Manager(field, a, b, dirA, dirB));
	
	waitFor(gp2);         // wait until simulator is ready
	
	double max = 0;
	Dist x;
	Time t = Sec(3);
	req.value.m_forward = Meter(1);
	req.publish();
	while( t.gt() )
	{
		t -= dt.value;
		if (gp2.value > max)
		{
			max = gp2.value;
			x = pose.value.x();
		}

		//cout << t.ms() << ": " << pose.value.x().mm() << " = " << gp2.value << endl;
		watchdog.publish();  // make step
		waitFor(gp2);         // wait for simulator
	}

	//cout << x;
	REQUIRE( x < Milim(910) && x > Milim(890)  );
}

AUTOTEST(testBallEating) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<int> inBalls(dirA, "in-balls");
	
	conf::Robot a("watchdog, ester-speed, pose-change, pose, ball-manip");
	conf::Robot b;
	Field field;
	field.setPalm(3,5);
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(inBalls);
	watchdog.publish();
	waitFor(inBalls);
	
	// TODO how to test this further?
}

AUTOTEST(testCamera) //{{{1
{
	Dir dirA; Dir dirB; 
	//Subs<Time> dt(dir, "time-change");
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Point> ballOffset(dirA, "ball-offset");
	Subs<Speed> req(dirA, "speed-requested");

	conf::Robot a("watchdog, ester-speed, pose-change, pose, ball-detect");
	conf::Robot b;
	Field field;
	field.setPalm(3,5);
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(ballOffset);// wait until simulator is ready
	
	req.value.m_angular = AngularSpeed(Deg(-180));
	req.publish();
	for (int i = 0; i < 55; i++)
	{
		watchdog.publish();  // step forward
		waitFor(ballOffset);
		//cout << ball.m_off.n() << endl;
	}
	REQUIRE( ballOffset.value.x().eq(Milim(560), Milim(20)) );
	REQUIRE( ballOffset.value.y().eq(Dist(), Milim(20)) );
}

AUTOTEST(testEnemy) //{{{1
{
	Dir dirA; Dir dirB; 
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Point> enemy(dirA, "enemy-offset");

	conf::Robot a("watchdog, ester-speed, pose-change, pose, enemy-detect");
	conf::Robot b;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	waitFor(enemy);

	// TODO who to test this further?
}
//}}}
} // namespace

