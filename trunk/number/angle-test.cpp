/** @file
*                  Tests for Angle
*
* @author (c) Zbynek Winkler 2004 <zw at robotika cz>
* 
* $Id$
*/

#include "util/unit-test.h"
#include <iostream>
#include "angle.h"

using namespace num;
using namespace std;

namespace {

AUTOTEST(test) //{{{1
{
	Rad a(M_PI);
	Deg b(90);
	Rad pi2(2*M_PI);
	Deg fullAngle(360);
	CPPUNIT_ASSERT( pi2 == fullAngle );
	CPPUNIT_ASSERT( (a + b + b) == pi2 );
	CPPUNIT_ASSERT( (b*2) == a );
	CPPUNIT_ASSERT( Rad(M_PI) == Deg(180) );
	CPPUNIT_ASSERT( Rad(M_PI).eq(Deg(180)) );
	CPPUNIT_ASSERT( Deg(179).eq(Deg(-179), Deg(3)) );
	
	a *= 18;
	//CPPUNIT_ASSERT( !(a <= Deg(180) && a >= Deg(-180)) );
	a.normalize();
	CPPUNIT_ASSERT( a.lt(Deg(180)) );
	CPPUNIT_ASSERT( a.gt(Deg(-180)) );
}
//}}}
}
