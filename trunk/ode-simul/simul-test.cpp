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
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	OdeSimul* simul = new OdeSimul(dir);
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


}
