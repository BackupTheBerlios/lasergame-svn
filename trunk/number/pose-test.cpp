/**
* @file
*           Tests for Position.h
*
* @author Zbynek Winkler
* 
* $Id$
*/

#include "util/unit-test.h"
#include "pose.h"
using namespace num;

#include <iostream>
using namespace std;

AUTOTEST(testOffset) //{{{1
{
	double e = 1e-4;
	Pose pos(Milim(100), Milim(100), Deg(45));
	Point goal(Milim(200), Milim(200));
	Point off = pos.offsetTo(goal);
	//printf("\n%.3lf %.3lf\n", toDouble(off.f()), toDouble(off.s()));
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, off.y().m(), e );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( M_SQRT2*.1, off.x().m(), e );
	
	pos.set(Milim(0), Milim(0), Deg(0));
	off = pos.offsetTo(goal);

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.2, off.y().m(), e );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.2, off.x().m(), e );

	pos.set(Milim(0), Milim(0), Deg(90));
	off = pos.offsetTo(goal);

	CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.2, off.y().m(), e );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.2, off.x().m(), e );

	pos.offsetBy(off);

	CPPUNIT_ASSERT_DOUBLES_EQUAL( goal.x().m(), pos.x().m(), e );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( goal.y().m(), pos.y().m(), e );

	pos.set(Milim(400), Milim(400), Deg(0));
	off = pos.offsetTo(Point(Milim(0), Milim(0)));

	CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.400, off.x().m(), e);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.400, off.y().m(), e);

}

AUTOTEST(testPoint) //{{{1
{
	double e = 1e-4;
	Point p(Milim(300), Milim(450));
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5408, toDouble(p.magnitude()), e);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.4132, toDouble(p.distanceTo(Point(Milim(532), Milim(108)))), e);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.9827, Angle(p).rad(), e);
}

AUTOTEST(testPose) //{{{1
{
	Pose b(Milim(0), Milim(0), Deg(45));
	
	b.advanceBy(Milim((int)::hypot(300, 300)));
	CPPUNIT_ASSERT( b.x().eq(Milim(300)) );
	CPPUNIT_ASSERT( b.y().eq(Milim(300)) );
	CPPUNIT_ASSERT( b.heading().eq(Deg(45)) );

	b.turnBy(Deg(45));
	CPPUNIT_ASSERT( b.heading().eq(Deg(90)) );

	b.advanceBy(Milim(300));
	CPPUNIT_ASSERT( b.x().eq(Milim(300)) );
	CPPUNIT_ASSERT( b.y().eq(Milim(600)) );
	
	Pose c;
	c.offsetBy(Milim(10), Milim(10));
	CPPUNIT_ASSERT( c.x().eq(Milim(10)) );
	CPPUNIT_ASSERT( c.y().eq(Milim(10)) );

	c.turnBy(Deg(90)).offsetBy(Milim(10), Milim(10));
	CPPUNIT_ASSERT( c.x().eq(Milim(0)) );
	CPPUNIT_ASSERT( c.y().eq(Milim(20)) );

	c.turnBy(Deg(90)).advanceBy(Milim(10)).slideBy(Milim(10));
	CPPUNIT_ASSERT( c.x().eq(Milim(-10)) );
	CPPUNIT_ASSERT( c.y().eq(Milim(10)) );
}

AUTOTEST(testCompare) //{{{1
{
	Point p1(Milim(600), Milim(600)), p2(Milim(600)+Dist::EPSILON(),
			Milim(600)-Dist::EPSILON());
	CPPUNIT_ASSERT( p1 == p1 );
	CPPUNIT_ASSERT( !(p1 == p2) );
	CPPUNIT_ASSERT( p1.eq(p2,Dist::EPSILON()*2) );
	CPPUNIT_ASSERT( p2.eq(p1,Dist::EPSILON()*2) );
}
//}}}

