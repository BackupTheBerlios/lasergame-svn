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
	conf::Robot a("watchdog"); Dir dirA;
	conf::Robot b; Dir dirB;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time> dt(dirA, "time-change");
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
	conf::Robot a("watchdog, ester-speed"); Dir dirA;
	conf::Robot b; Dir dirB;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	Subs<bool> watchdog(dirA, "watchdog");

	Subs< std::pair<num::Time, num::Speed> > cur(dirA, "time-change+speed-current");
	Subs<Speed> req(dirA, "speed-requested");
	
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
	conf::Robot a("watchdog, ester-speed, pose-change"); Dir dirA;
	conf::Robot b; Dir dirB;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	Subs<bool> watchdog(dirA, "watchdog");

	Subs<Time> dt(dirA, "time-change");
	Subs<Pose> dp(dirA, "pose-change");
	Subs<Speed> req(dirA, "speed-requested");
	
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
	conf::Robot a("watchdog, ester-speed, pose-change"); Dir dirA;
	conf::Robot b; Dir dirB;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	Subs<bool> watchdog(dirA, "watchdog");

	Subs<Time> dt(dirA, "time-change");
	Subs<Pose> dp(dirA, "pose-change");
	Subs<Speed> req(dirA, "speed-requested");
	
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
	conf::Robot a("watchdog, ester-speed, pose-change, pose"); Dir dirA;
	conf::Robot b; Dir dirB;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	Subs<bool> watchdog(dirA, "watchdog");

	Subs<Time>   dt(dirA, "time-change");
	Subs<Speed> req(dirA, "speed-requested");
	Subs<Pose> pose(dirA, "pose");
	
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
	Dir dirA;
	conf::Robot a("watchdog, ester-speed, pose-change, pose, floor-color");
	conf::Robot b; Dir dirB;
	Field field;
	Task e(new Manager(field, a, b, dirA, dirB));

	Subs<bool> watchdog(dirA, "watchdog");
	Subs<FloorColor> floorColor(dirA, "floor-color");
	
	waitFor(floorColor); // wait until simulator is ready
	watchdog.publish();  // make step
	waitFor(floorColor); // wait for floor color message to come in
	
	// TODO how to test this further?
}
//}}}

#if 0
AUTOTEST(testGP2) //{{{1
{
	Dir dirA;
	conf::Robot a("watchdog, ester-speed, pose-change, pose, gp2top");
	conf::Robot b; Dir dirB;
	Field field;
	field.setPalm(3,5);
	Task e(new Manager(field, a, b, dirA, dirB));
	
	Subs<bool> watchdog(dirA, "watchdog");
	Subs<Time> dt(dirA, "time-change");
	Subs<Speed> req(dirA, "speed-requested");
	Subs<Pose> pose(dirA, "pose");
	Subs<double> gp2(dirA, "gp2top");

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
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<int> numBallsIn(dir, "num-balls-in");
	Field field;
	field.setBall(0,1,2);
	Task e(new Simul(dir, &field, 0));
	waitFor(dt);         // wait until simulator is ready
	watchdog.publish();  // step forward
	waitFor(numBallsIn); // wait for the message to come in
	
	// TODO who to test this further?
}

AUTOTEST(testCamera) //{{{1
{
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Point> ball(dir, "ball");
	Subs<Speed> req(dir, "speed-requested");
	
	Field field;
	Task e(new Simul(dir, &field, 0));
	waitFor(dt);         // wait until simulator is ready
	
	for (int i = 0; i < 20; i++)
	{	
		watchdog.publish();  // step forward
		waitFor(dt);
	}
	req.value.m_angular = AngularSpeed(Deg(-180));
	req.publish();
	for (int i = 0; i < 55; i++)
	{
		watchdog.publish();  // step forward
		waitFor(dt);
		//cout << ball.m_off.n() << endl;
	}
	REQUIRE( ball.value.x().eq(Milim(560), Milim(20)) );
	REQUIRE( ball.value.y().eq(Dist(), Milim(20)) );
}

AUTOTEST(testEnemy) //{{{1
{
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Point> enemy(dir, "enemy");
	
	Field field;
	Task e(new Simul(dir, &field, 0));
	waitFor(dt);         // wait until simulator is ready
	watchdog.publish();  // make step
	waitFor(enemy);      // wait for the message to come in
	
	// TODO who to test this further?
}


//}}}
#endif
} // namespace

