// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include <iostream>
using namespace std;

#include "simul.h"
#include "robots/four-wheeler.h"
#include "util/assert.h"
#include "util/unit-test.h"
using namespace num;
using namespace msg;

namespace {



AUTOTEST(testTime) //{{{1
{
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	OdeSimul* simul = new OdeSimul(dir);
	OdeRobot* bot = new FourWheeler(dir);
	simul->addRobot(bot);
	Task e(simul);
	int i = 0;
	for ( ; i < 10; i++)
	{
		waitFor(dt);
		REQUIRE( dt.value == MSec(5) );
		watchdog.publish();
	}
	REQUIRE( i == 10 );
}

AUTOTEST(testMove) //{{{1
{
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
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
		i++;
		watchdog.publish();
		waitFor(dt);
		//cout << t.ms() << ": " << d.mm() << endl;
	}
	REQUIRE( i == 241 );
}

}
