// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include <iostream>
using namespace std;

#include "simul.h"
#include "util/assert.h"
#include "util/unit-test.h"
using namespace num;
using namespace msg;

namespace {

AUTOTEST(testTime) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Field field;
	Task e(Simul::fac(dir, &field, 0));
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
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Speed> cur(dir, "speed-current");
	Subs<Speed> req(dir, "speed-requested");
	
	Field field;
	Task e(Simul::fac(dir, &field, 0));
	waitFor(dt);
	int i = 0;

	// accelerate
	Time t = MSec(500);
	req.value.m_linear = Milim(300);
	//cout << req.value.m_linear.mm() << endl; 
	req.publish();
	while( t.gt() && cur.value.m_linear.lt(LinearSpeed(Milim(250))) )
	{
		t -= dt.value;
		i++;
		//cout << t.ms() << ": " << cur.value.m_linear.mm() << endl;
		watchdog.publish();
		waitFor(dt);
	}
	//cout << endl;
	REQUIRE( !cur.value.m_linear.lt(LinearSpeed(Milim(250))) );

	// decelerate
	t = MSec(600);
	req.value.m_linear = Milim(0);
	req.publish();
	while( t.gt() && cur.value.m_linear.gt() )
	{
		t -= dt.value;
		i++;
		//cout << t.ms() << ": " << currentSpeed.m_v.mm() << endl;
		watchdog.publish();
		waitFor(dt);
	}
	REQUIRE( !cur.value.m_linear.gt() );
	REQUIRE( i == 132); // 136
}

AUTOTEST(testMove) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Pose> dp(dir, "pose-change");
	Subs<Speed> req(dir, "speed-requested");
	
	Field field;
	Task e(Simul::fac(dir, &field, 0));
	waitFor(dt);
	int i = 0;
	
	Time t = Sec(2);
	Meter d(1);
	req.value.m_linear = Milim(1070);
	req.publish();
	while( t.gt() && d.gt() )
	{
		t -= dt.value;
		d -= dp.value.x();
		i++;
		watchdog.publish();
		waitFor(dt);
		//cout << t.ms() << ": " << d.mm() << endl;
	}
	REQUIRE( !d.gt() );
	REQUIRE( i == 241 );
}

AUTOTEST(testTurn) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Pose> dp(dir, "pose-change");
	Subs<Speed> req(dir, "speed-requested");
	Field field;
	Task e(Simul::fac(dir, &field, 0));
	int i = 0;
	waitFor(dt);
	
	Time t = Sec(2);
	Deg a(180);
	req.value.m_angular = AngularSpeed(Deg(180));
	req.publish();
	while( t.gt() && a.gt() )
	{
		t -= dt.value;
		a -= dp.value.heading();
		i++;
		//cout << t.ms() << ": " << a.deg() << endl;
		watchdog.publish();
		waitFor(dt);
	}
	REQUIRE( a.lt() );
	REQUIRE( i == 217 );
}

AUTOTEST(testFloorColor) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<FloorColor> floorColor(dir, "floor-color");
	
	Field field;
	Task e(Simul::fac(dir, &field, 0));
	waitFor(dt);         // wait until simulator is ready
	watchdog.publish();  // make step
	waitFor(floorColor); // wait for floor color message to come in
	
	// TODO who to test this further?
}

AUTOTEST(testGP2) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Speed> req(dir, "speed-requested");
	Subs<Pose> pose(dir, "true-pose");
	Subs<double> gp2(dir, "gp2top");

	Field field;
	field.setPalm(3,5);
	Task e(Simul::fac(dir, &field, 0));
	waitFor(dt);         // wait until simulator is ready
	
	double max = 0;
	Dist x;
	Time t = Sec(3);
	req.value.m_linear = Meter(1);
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
		waitFor(dt);         // wait for simulator
	}

	//cout << x;
	REQUIRE( x < Milim(910) && x > Milim(890)  );
}

AUTOTEST(testBallEating) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<int> numBallsIn(dir, "num-balls-in");
	Field field;
	field.setBall(0,1,2);
	Task e(Simul::fac(dir, &field, 0));
	waitFor(dt);         // wait until simulator is ready
	watchdog.publish();  // step forward
	waitFor(numBallsIn); // wait for the message to come in
	
	// TODO who to test this further?
}
//}}}

#if 0
//{{{1 testTruePose()
struct TestTruePose : public Action //{{{2
{
	virtual int main()
	{
		using num::Milim; using num::Deg; using num::Sec; using num::Time; using num::ASpeed; using num::Meter;
		msg::Speed requestedSpeed;
		Regs r1("cz.robotika.ester.requested_speed", &requestedSpeed);
		msg::TimeChange dt;
		msg::Pose pose;
		Subs s1("time_change", &dt);
		Subs s2("cz.robotika.ester.true_pose", &pose);
		execute();

		Time t = Sec(1);
		requestedSpeed.m_time = dt.m_time;
		requestedSpeed.m_v = Meter(1);
		while( t.gt() )
		{
			t -= dt.m_dt;
			//cout << t.ms() << ": " << pose.m_pose.x().mm() << endl;
			execute();
		}
		ASSERT( pose.m_pose.x() > Milim(500) );
		return 0;
	}
};

AUTOTEST(testTruePose) //{{{2
{
	EsterSimul t2;
	TestTruePose t3;
	g_main.waitForAny();
}
//}}}

//{{{1 testCamera()
struct TestCamera : public Action //{{{2
{
	virtual int main()
	{
		msg::Offset ballPos;
		Subs s("cz.robotika.ester.ball_pos", &ballPos);
		msg::Speed requestedSpeed;
		Regs r1("cz.robotika.ester.requested_speed", &requestedSpeed);
		for (int i = 0; i < 20; i++)
			execute();
		requestedSpeed.m_time = ballPos.m_time;
		requestedSpeed.m_omega = ASpeed(Deg(-180));
		for (int i = 0; i < 55; i++)
		{
			execute();
			//cout << ballPos.m_off.n() << endl;
		}
		ASSERT( ballPos.m_off.m_f.eq(Milim(560), Milim(15)) );
		ASSERT( ballPos.m_off.m_s.eq(Dist(), Milim(15)) );
		return 0;
	}
};

AUTOTEST(testCamera) //{{{2
{
	EsterSimul e(0);
	TestCamera tc;
	g_main.waitForAny();
}
//}}}1
#endif
} // namespace

