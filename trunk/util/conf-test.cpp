#include "util/unit-test.h"
#include "util/conf.h"

using namespace conf;

AUTOTEST(create)
{
	Robot a;
	Robot b("watchdog, ester-speed, pose-change");
	REQUIRE( b.size() == 3 );
	REQUIRE( b[0] == "watchdog" );
	REQUIRE( b[1] == "ester-speed" );
	REQUIRE( b[2] == "pose-change" );
}

AUTOTEST(iterate)
{
	Robot r;
	r.push_back("ester-speed");
	r.push_back("pose-change");
	r.push_back("pose");
	int i = 0;
	for (Robot::iterator iter = r.begin(); iter != r.end(); ++iter, i++)
		;
	REQUIRE( i == 3 );
}

