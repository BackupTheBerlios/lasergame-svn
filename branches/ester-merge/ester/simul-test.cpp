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
	Task e(Simul::fac(dir, field, 0));
	int i = 0;
	for ( ; i < 10; i++)
	{
		waitFor(dt);
		REQUIRE( dt.value == MSec(5) );
		watchdog.publish();
	}
	REQUIRE( i == 10 );
}
//}}}

AUTOTEST(testFSpeed) //{{{1
{
	Subs<int> dir;
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Speed> cur(dir, "speed-current");
	Subs<Speed> req(dir, "speed-requested");
	
	Field field;
	Task e(Simul::fac(dir, field, 0));
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
//}}}

#if 0
//{{{1 testMove()
struct TestMove : public Action //{{{2
{
	int m_count;
	virtual int main()
	{
		using num::Milim; using num::Meter; using num::Sec; using num::Time; using num::FSpeed;
		msg::TimeChange dt;
		Subs s1("time_change", &dt);
		msg::Speed requestedSpeed;
		Regs r1("cz.robotika.ester.requested_speed", &requestedSpeed);
		msg::PoseChange dp;
		Subs s2("cz.robotika.ester.pose_change", &dp);
		m_count = 0;
		execute();

		Time t = Sec(2);
		Meter d(1);
		requestedSpeed.m_time = dt.m_time;
		requestedSpeed.m_v = Milim(1070);
		while( t.gt() && d.gt() )
		{
			execute();
			t -= dt.m_dt;
			d -= dp.m_dp.m_f;
			m_count++;
			//cout << t.ms() << ": " << d.mm() << endl;
		}
		ASSERT( !d.gt() );
		return 0;
	}
};

AUTOTEST(testMove) //{{{2
{
	EsterSimul t2;
	TestMove t3;
	g_main.waitForAny();
	CPPUNIT_ASSERT_EQUAL( 241, t3.m_count );
}
//}}}

//{{{1 testTurn()
struct TestTurn : public Action //{{{2
{
	virtual int main()
	{
		using num::Milim; using num::Deg; using num::Sec; using num::Time; using num::ASpeed;
		msg::TimeChange dt;
		Subs s1("time_change", &dt);
		msg::Speed requestedSpeed;
		Regs r1("cz.robotika.ester.requested_speed", &requestedSpeed);
		msg::PoseChange dp;
		Subs s2("cz.robotika.ester.pose_change", &dp);
		execute();

		Time t = Sec(2);
		Deg a(180);
		requestedSpeed.m_time = dt.m_time;
		requestedSpeed.m_omega = ASpeed(Deg(180));
		while( t.gt() && a.gt() )
		{
			execute();
			t -= dt.m_dt;
			a -= dp.m_dp.m_a;
			//cout << t.ms() << ": " << a.deg() << endl;
		}
		ASSERT( a.lt() );
		return 0;
	}
};

AUTOTEST(testTurn) //{{{2
{
	EsterSimul t2;
	TestTurn t3;
	g_main.waitForAny();
}
//}}}

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

//{{{1 testFloorColor()
struct TestFloorColor : public Action //{{{2
{
	virtual int main()
	{
		//execute();
		// don't know how to test :-(
		return 0;
	}
};

AUTOTEST(testFloorColor) //{{{2
{
	EsterSimul t2;
	TestFloorColor t3;
	g_main.waitForAny();
}
//}}}

//{{{1 testGP2()
struct TestGP2 : public Action //{{{2
{
	virtual int main()
	{
		using namespace num;
		msg::Speed requestedSpeed;
		Regs r1("cz.robotika.ester.requested_speed", &requestedSpeed);
		msg::TimeChange dt;
		msg::Pose pose;
		msg::Number gp2;
		Subs s1("time_change", &dt);
		Subs s2("cz.robotika.ester.true_pose", &pose);
		Subs s3("cz.robotika.ester.gp2.top", &gp2);
		Number m_max = 0;
		Dist m_x;
		execute();		

		Time t = Sec(3);
		requestedSpeed.m_time = dt.m_time;
		requestedSpeed.m_v = Meter(1);
		while( t.gt() )
		{
			t -= dt.m_dt;
			if (gp2.m_num > m_max)
			{
				m_max = gp2.m_num;
				m_x = pose.m_pose.x();
			}

			//cout << t.ms() << ": " << pose.m_pose.x().mm() << " = " << toDouble(gp2.m_num) << endl;
			execute();
		}

		ASSERT( m_x < Milim(910) && m_x > Milim(890)  );
		return 0;
	}
};

AUTOTEST(testGP2) //{{{2
{
	EsterSimul t2(0);
	t2.setPalm(3,5);
	TestGP2 t3;
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

