// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include <iostream>
using namespace std;

#include "util/assert.h"
#include "util/unit-test.h"
#include "ball.h"
#include "../simul.h"

using namespace num;
using namespace msg;

namespace {


AUTOTEST(testDrop) //{{{1
{
	using namespace num3D;
	Subs<int> dir(0);
	Subs<Time> dt(dir, "time-change");
	Subs<int> watchdog(dir, "watchdog");
	
	OdeSimul* simul = new OdeSimul(dir);
	Ball* ball = new Ball(Milim(100),1);
	simul->addObject(ball, Pose3D(Point3D(Milim(300),Milim(0),Meter(1)),Quaternion()));
	Task e(simul);
	waitFor(dt);
	int i = 0;
	
	Time t = Sec(1.5);
	while( t.gt() )
	{
		t -= dt.value;
		i++;
		watchdog.publish();
		waitFor(dt);
		cout << t.ms() << ": Ball height: " << ball->getPose3D().point().z().mm() << endl;
	}
	REQUIRE( ball->getPose3D().point().z().mm() == 100 );
}

}

