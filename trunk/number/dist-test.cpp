/** @file
*                  Tests for Dist
*
* @author (c) Zbynek Winkler 2004 <zw at robotika cz>
* @author (c) Jaroslav Sladek 2004 <jup at matfyz cz>
* 
* $Id$
*/

#include "dist.h"
#include "Double.h"
#include "util/unit-test.h"

using namespace num;

namespace {

typedef Double Number;	
typedef DistT<Number> Dist;
typedef MilimT<Number> Milim;
typedef MeterT<Number> Meter;

AUTOTEST(test) //{{{1
{
	Milim a;
	CPPUNIT_ASSERT( !a.gt() );
	CPPUNIT_ASSERT( !a.lt() );
	int counter = 100;
	for (int i = 1; i<=counter; i++)
	{
		a += Milim(i);
	}
	CPPUNIT_ASSERT( !a.gt((Milim(1) + Milim(counter))*counter/2) ) ;
	CPPUNIT_ASSERT( !a.lt((Milim(1) + Milim(counter))*counter/2) ) ;
}

AUTOTEST(testCompare)
{
	Milim a(Number(1000)), c(Number(1500)), d(Number(-500)), e;
	Meter b(Number(1));
	CPPUNIT_ASSERT ( a == b);
	CPPUNIT_ASSERT ( a < c );
	CPPUNIT_ASSERT ( c > a );
	CPPUNIT_ASSERT ( a <= c );
	CPPUNIT_ASSERT ( c >= a );
	CPPUNIT_ASSERT ( a <= b );
	CPPUNIT_ASSERT ( a >= b );
	
	CPPUNIT_ASSERT ( !a.eq() );
	CPPUNIT_ASSERT ( a.gt() );
	CPPUNIT_ASSERT ( !d.eq() );
	CPPUNIT_ASSERT ( !d.gt() );
	CPPUNIT_ASSERT ( e.eq() );
	CPPUNIT_ASSERT ( !e.gt() );
}

AUTOTEST(testArit)
{
	int i;
	Meter a(Number(1,1000));

	for (i = 0; i < 10; i++)
	  a *= 2;

	CPPUNIT_ASSERT( a.eq(Milim(1024)) );
	
	for (i = 0; i < 10; i++)
	  a /= 2;

	CPPUNIT_ASSERT( a == Milim(1) );

	for (i = 0; i < 999; i++)
	  a += Milim(1);

	CPPUNIT_ASSERT( a.eq(Milim(1000)) );

	for (i = 0; i < 1000; i++)
	  a -= Meter(Number(1,100));

	CPPUNIT_ASSERT( a.eq(-Milim(9000)) );

	CPPUNIT_ASSERT_DOUBLES_EQUAL( a.m(), -9, Dist::EPSILON().m() );
}
//}}}
}
