/** @file
*        Tests for various speed related classes
*
* @author (c) Zbynek Winkler 2004 <zw at robotika cz>
* 
* $Id$
*/

#include "util/unit-test.h"
#include "speed.h"
using namespace num;

#include <iostream>
using namespace std;

AUTOTEST(test1) //{{{1
{
	LinearSpeed l;
	l = Milim(300);
	REQUIRE( l.mm() == Milim(300).mm() );
#if 0
	double v = 3.7;        // m/s == pocatecni rychlost
	double b = 3.7;        // konstanta
	double s = v/b;        // brzdna draha
	double t = log(v+1)/b; // brzdny cas

	//cout << "stop dist: " << s << endl;
	//cout << "stop time: " << t << endl;

	double dt = 0.005;     // delka kroku
	double totalTime = 0;
	double totalDist = 0;
	while (v > 0)
	{
		t = log(v+1)/b;
		v = exp((t-dt)*b) - 1;
		totalTime += dt;
		totalDist += dt*v;
	}
	//cout << "total time: " << totalTime << endl;
	//cout << "total dist: " << totalDist << endl;
#endif
}

AUTOTEST(test2) //{{{1
{
#if 0
	double v = 1;        // m/s == pocatecni rychlost
	double b = 3.7;        // konstanta
	double s = v/b;        // brzdna draha
	double t = log(v+1)/b; // brzdny cas

	cout << "stop dist: " << s << endl;
	cout << "stop time: " << t << endl;

	double dt = 0.005;     // delka kroku
	double totalTime = 0;
	double totalDist = 0;
	int step = 0;
	while (v > 0)
	{
		v = exp((t-dt*step++)*b) - 1;
		totalTime += dt;
		totalDist += dt*v;
	}
	cout << "total time: " << totalTime << endl;
	cout << "total dist: " << totalDist << endl;
#endif
}
//}}}

