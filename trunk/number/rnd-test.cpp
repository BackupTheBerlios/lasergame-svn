#include "util/unit-test.h"
#include <iostream>
#include "rnd.h"
#include "util/util.h" // ARRAY_SIZE
using namespace std;
using namespace num;

AUTOTEST(testRand) //{{{1
{
	Rnd rnd;
	double s = 0;
	for (int j = 0; j < 10; j++)
	{
		rnd.setSeed(j*91);
		for (int i = 0; i < 256; i++)
			s += rnd.uniformD();
		//printf("\n!!! %lf !!!\n\n", s/256);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, s/256, 0.1 );
	}
}

AUTOTEST(testDeterminism) //{{{1
{
	Rnd rnd;
	double r[] = 
	{
		-0.997498,
		0.127136,
		-0.613403,
		0.617432,
		0.169983,
		-0.0402832,
		-0.299438,
		0.79187,
		0.64563,
		0.493164,
	};
	for (int j = 0; j < ARRAY_SIZE(r); j++)
	{
		double d = rnd.uniformD();
		CPPUNIT_ASSERT_DOUBLES_EQUAL( r[j], d, 1e-4 );
		//cout << rnd.uniformD() << endl;
	}
}

