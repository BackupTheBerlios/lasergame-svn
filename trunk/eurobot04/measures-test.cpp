#include <iostream>
using namespace std;

#include "measures.h"
#include "util/unit-test.h"

AUTOTEST(testGetColor) //{{{1
{
	using namespace measures;
	Point p;
	Point tile(TILE()-Milim(1), TILE()-Milim(1));
	CPPUNIT_ASSERT_EQUAL( BLACK, getColor(p, 0) );
	CPPUNIT_ASSERT_EQUAL( BLACK, getColor(p += Point(Milim(150), Milim(150)), 0) );
	CPPUNIT_ASSERT_EQUAL( BLACK, getColor(p += tile, 0) );
	CPPUNIT_ASSERT_EQUAL( WHITE, getColor(p += Point(TILE(), Dist()), 0) );
	CPPUNIT_ASSERT_EQUAL( WHITE, getColor(p += tile, 0) );
	CPPUNIT_ASSERT_EQUAL( WHITE, getColor(Point(SIZE_X()-Milim(1), SIZE_Y()-Milim(1)), 0) );
}

AUTOTEST(testGetColorInv) //{{{1
{
	using namespace measures;
	Point p;
	Point tile(TILE(), TILE());
	CPPUNIT_ASSERT_EQUAL( WHITE, getColor(p, 1) );
	CPPUNIT_ASSERT_EQUAL( WHITE, getColor(p += Point(Milim(150), Milim(150)), 1) );
	CPPUNIT_ASSERT_EQUAL( WHITE, getColor(p += tile, 1) );
	CPPUNIT_ASSERT_EQUAL( BLACK, getColor(p += Point(TILE(), Dist()), 1) );
	CPPUNIT_ASSERT_EQUAL( BLACK, getColor(p += tile, 1) );
	CPPUNIT_ASSERT_EQUAL( BLACK, getColor(Point(SIZE_X()-Milim(1), SIZE_Y()-Milim(1)), 1) );
}
//}}}

