//{{{1 #include
#include "util/unit-test.h"
#include <fstream>
#include <iostream>
//#include <iomanip>
#include <algorithm>
#include "mcl.h"
#include "dre.h"
#include "lvr.h"
//#include "OdoFilter.h"
#include "mcldist.h"

using namespace std;
using namespace num;
using namespace mcl;

namespace
{
AUTOTEST(testMainLoop) //{{{1
{
	MCL::iterator s;
	MCL mcl(2);
	Rnd my_rnd;
	Pose start(Milim(150), Milim(150), Deg(45));
	mcl.init(start, my_rnd);

	Sample pos;
	mcl.getResult(pos);
//	printf("\nx: %lf y: %lf a: %lf w: %lf\n", toDouble(pos.x()), toDouble(pos.y()), toDouble(pos.heading()), toDouble(pos.w()));
	
	CPPUNIT_ASSERT( start.eq(pos, Pose(Milim(5), Milim(5), Deg(5))) );

	for (s = mcl.begin(); s != mcl.end(); s++)
	{
		s->w() *= 1.2;
	}

	mcl.normalize();

	for (s = mcl.begin(); s != mcl.end(); s++)
	{
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0,  toDouble(s->w()), 0.0001);
	}

}
#if 0
AUTOTEST(testOdoBasic) //{{{1
{
	OdoBasic odo(Milim(195));
	Pose pos;
	Rnd my_rnd;

	odo.init(Milim(3), Milim(3));
	odo.apply(&pos, my_rnd);

	CPPUNIT_ASSERT( pos.x() == Milim(3) );
	CPPUNIT_ASSERT( pos.y() == Milim(0) );
	CPPUNIT_ASSERT( pos.heading() == Deg(0) );

	odo.init(Milim(-3), Milim(3));
	odo.apply(&pos, my_rnd);

	CPPUNIT_ASSERT( pos.x() == Milim(3) );
	CPPUNIT_ASSERT( pos.y() == Milim(0) );
	CPPUNIT_ASSERT( pos.heading().eq(Rad(6./(2.*195.))) );
}

AUTOTEST(testOdoNextGen) //{{{1
{
	Rnd rnd;
	OdoNextGen odo(0,0,0);
	Pose ref, test, dp;
	for (int j = 0; j < 10; j++)
	{
		dp = Pose();
		for (int i = 0; i < 10; i++)
		{
			ref.advanceBy(Milim(3));
			dp.advanceBy(Milim(3));
			ref.turnBy(Deg(1));
			dp.turnBy(Deg(1));
		}
		odo.init(dp.x(), dp.y(), dp.heading());
		odo.apply(&test, rnd);
	}
	//cout << ref << endl;
	//cout << test << endl;

	CPPUNIT_ASSERT( test.x().eq( ref.x() ) );
	CPPUNIT_ASSERT( test.y().eq( ref.y() ) );
	CPPUNIT_ASSERT( test.heading().eq( ref.heading() ) );
}
#endif
AUTOTEST(testResampleDRE) //{{{1 ////////////////////
{
	Rnd rnd;
	MCL mcl(1<<8);
	DRE resample;
	MCL::iterator s;
	//rnd.setSeed(1410398721);
	for (int runcount = 0; runcount < 10; runcount++)
	{
		//std::cout << rnd << std::endl;
		mcl.init(Pose(), rnd);
		for (s = mcl.begin(); s != mcl.end(); s++) s->w() = (rnd() + 1)*1.5; // weights from (0,3)
		mcl.normalize();
		MCL mcl_before(mcl);
		resample(mcl);
		for (s = mcl.begin(); s != mcl.end(); s++)
			CPPUNIT_ASSERT( s->w() < DRE::w_max() && s->w() > DRE::w_min() );
		
		//std::cout << mclDist(mcl_before, mcl) << std::endl;
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, mclDist(mcl_before, mcl), 0.04 );
	}
}

AUTOTEST(testResampleLVR) //{{{1 ////////////////////
{
	Rnd rnd;
	MCL mcl(1<<8);
	LVR resample;
	for (int runcount = 0; runcount < 10; runcount++)
	{
		mcl.init(Pose(), rnd);
		MCL::iterator s;
		for (s = mcl.begin(); s != mcl.end(); s++)
			s->w() = (rnd() + 1)*1.5; // random weights from (0,3)
		mcl.normalize();
		MCL mcl_before(mcl);
		std::sort(mcl_before.begin(), mcl_before.end(), num::cmpX<Sample::type>);
		resample(mcl, rnd);
		std::sort(mcl.begin(), mcl.end(), num::cmpX<Sample::type>);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, mclDist(mcl_before, mcl), 0.4 );
		//cout << mcl << endl;
		for (s = mcl.begin(); s != mcl.end(); s++)
			CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, toDouble(s->w()), 1e-8);
	}
}
#if 0
AUTOTEST(testMCL) //{{{1
{
	Rnd rnd;
	Pose test, dp;
	MCL localize(400);
	localize.init(Pose(), rnd);
	OdoNextGen odo(0,0,0);
	DRE resample;

	for (int i = 0; i < 100; i++)
	{
		dp = Pose();
		for (int j = 0; j < 5; j++)
		{
			test.advanceBy(Milim(3));
			test.turnBy(Deg(1));
			dp.advanceBy(Milim(3));
			dp.turnBy(Deg(1));
		}
		odo.init(dp.x(), dp.y(), dp.heading());
		for (MCL::iterator i = localize.begin(); i < localize.end(); i++)
			odo.apply(&(*i), rnd);
		localize.normalize();
		resample(localize);
		CPPUNIT_ASSERT( test.eq(localize.getResult(), Pose(Milim(2), Milim(2), Deg(1))) );
	}
}
#endif
AUTOTEST(testPartSort) //{{{1
{
	//void partSort(int& b_begin, int& b_end);
	Rnd rnd;
	MCL mcl(1<<8);
	DRE resample;
	//rnd.setSeed(1410398721);
	for (int runcount = 0; runcount < 20; runcount++)
	{
		mcl.init(Pose(), rnd);
		MCL::iterator s;
		for (s = mcl.begin(); s != mcl.end(); s++)
			s->w() = (rnd() + 1)*1.5; // random weights from (0,2)*1.5 -> (0,3)
		size_t i_min, i_max;
		resample.partSort(mcl, i_min, i_max);
		//std::cout << i_min+1 << "\t" << i_max+1 << std::endl; // outputs line numbers
		//std::ofstream("sorted.dat") << mcl;
		size_t j;
		for (j=0; j < i_min; j++)
			CPPUNIT_ASSERT( mcl[j].w() < DRE::w_min() );
		for ( ; j < i_max; j++)
			CPPUNIT_ASSERT( mcl[j].w() >= DRE::w_min() && mcl[j].w() < DRE::w_max() );
		for ( ; j < mcl.size(); j++)
			CPPUNIT_ASSERT( mcl[j].w() >= DRE::w_max() );
	}
}

AUTOTEST(testMCLDist) //{{{1
{
	Rnd rnd;
	for (int runcount = 0; runcount < 10; runcount++)
	{
		MCL mcl1(1<<8);
		mcl1.init(Pose(), rnd);
		MCL::iterator s;
		for (s = mcl1.begin(); s != mcl1.end(); s++)
			s->w() = rnd() + 1; // random weights from (0,2)
		
		//std::ofstream("rnd.dat") << mcl1;
		MCL mcl2(mcl1);
	
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, mclDist(mcl1, mcl2), 1e-8 );
	}
}
//}}}
}
