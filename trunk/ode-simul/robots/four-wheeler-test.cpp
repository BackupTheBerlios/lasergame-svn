// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include <iostream>
using namespace std;

#include "../simul.h"
#include "four-wheeler.h"
#include "util/assert.h"
#include "util/unit-test.h"
using namespace num;
using namespace msg;

namespace {

AUTOTEST(testLinearSpeed) //{{{1
{
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Speed> cur(dir, "speed-current");
	Subs<Speed> req(dir, "speed-requested");

	OdeSimul* simul = new OdeSimul(dir);
	OdeRobot* bot = new FourWheeler(dir);
	simul->addRobot(bot);
	Task e(simul);
	waitFor(dt);
	int i = 0;

	// accelerate
	Time t = MSec(500);
	req.value.m_forward = Milim(300);
	//cout << req.value.m_forward.mm() << endl; 
	req.publish();
	while( t.gt() && cur.value.m_forward.lt(LinearSpeed(Milim(250))) )
	{
		t -= dt.value;
		i++;
		//cout << t.ms() << ": " << cur.value.m_forward.mm() << endl;
		watchdog.publish();
		waitFor(dt);
	}
	//cout << endl;
	REQUIRE( !cur.value.m_forward.lt(LinearSpeed(Milim(250))) );

	// decelerate
	t = MSec(600);
	req.value.m_forward = Milim(0);
	req.publish();
	while( t.gt() && cur.value.m_forward.gt() )
	{
		t -= dt.value;
		i++;
		//cout << t.ms() << ": " << currentSpeed.m_v.mm() << endl;
		watchdog.publish();
		waitFor(dt);
	}
	REQUIRE( !cur.value.m_forward.gt() );
	//REQUIRE( i == 132); // 136
}


AUTOTEST(testMove) //{{{1
{
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	Subs<Pose> dp(dir, "pose-change");
	Subs<Speed> req(dir, "speed-requested");
	
	OdeSimul* simul = new OdeSimul(dir);
	OdeRobot* bot = new FourWheeler(dir);
	simul->addRobot(bot);
	Task e(simul);
	waitFor(dt);
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
		waitFor(dt);
		//cout << t.ms() << ": " << d.mm() << endl;
	}
	REQUIRE( !d.gt() );
	//REQUIRE( i == 241 );
}




}
