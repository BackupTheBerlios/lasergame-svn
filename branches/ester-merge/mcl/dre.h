#ifndef MCL_DRE_H_INCLUDED
#define MCL_DRE_H_INCLUDED 1

/** @file
*              DRE declaration
*
* @author Zbynek Winkler (c) 2003, 2004 <zw at robotika cz>
* @author Vojtech Pavlik (c) 2003 <vojtech at ucw cz>
*
* $Id$
*/

#include <algorithm>
#include "mcl.h"
#include "util/assert.h"
#include "util/osdep.h" // UNUSED

namespace mcl
{
	class DRE
	{
		typedef double Number;
		Number pivot1, pivot2, pivot3, pivot4, pivot5;
	
		struct cmp //{{{
		{
			Number m_pivot;
			cmp(Number a) : m_pivot(a) {}	
			bool operator () (const Sample & s1) { return s1.w() < m_pivot; }
		}; //}}}
	
	public:
		void partSort(MCL & a_mcl, size_t & b_begin, size_t & b_end) //{{{
		{
			using namespace std;
			MCL::iterator i3 = partition(a_mcl.begin(), a_mcl.end(), cmp(pivot3));
			ASSERT( i3 == a_mcl.end() || i3->w() >= pivot3 );
			
			MCL::iterator i1 = partition(a_mcl.begin(), i3, cmp(pivot1));
			ASSERT( i1 != a_mcl.end() && i1->w() >= pivot1 );
			b_begin = partition(a_mcl.begin(), i1, cmp(w_min())) - a_mcl.begin();
			ASSERT( a_mcl[b_begin].w() >= w_min() );
			MCL::iterator i2 UNUSED = partition(i1, i3, cmp(pivot2));
			ASSERT( i2 != a_mcl.end() && i2->w() >= pivot2 );
			
			MCL::iterator i5 = partition(i3, a_mcl.end(), cmp(pivot5));
			ASSERT( i5 == a_mcl.end() || i5->w() >= pivot5 );
			b_end = partition(i5, a_mcl.end(), cmp(w_max())) - a_mcl.begin();
			ASSERT( b_end == a_mcl.size() || a_mcl[b_end].w() >= w_max() );
			MCL::iterator i4 UNUSED = partition(i3, i5, cmp(pivot4));
			ASSERT( i4 == a_mcl.end() || i4->w() >= pivot4 );
		} //}}}
		DRE() //{{{
		{
			Number inc = (w_max() - w_min()) / 6;
			pivot1 = w_min() + inc;
			pivot2 = pivot1 + inc;
			pivot3 = pivot2 + inc;
			pivot4 = pivot3 + inc;
			pivot5 = pivot4 + inc;
			ASSERT( num::eq((pivot5 + inc), w_max()) );
		} //}}}
		/// Threshold, when weight of an item drops bellow it the sample must die
		static double w_min() { return 0.25; }
		/// Threshold, when weight of an item raises above it must eat the dead
		static double w_max() { return 2.0; }
	
		void operator () (MCL & a_mcl) //{{{
		{
			size_t i_max, i_min;
			partSort(a_mcl, i_min, i_max);
		
			size_t num = 0;
			size_t i;
			for (i = i_max; i < a_mcl.size(); i++)
			{
				int count = num::toInt(a_mcl[i].w());
				a_mcl[i].w() /= count;
				for (size_t j = num; j < num+count-1; j++)
					a_mcl[j] = a_mcl[i];
				num += count-1;
			}
			for (i = num; i < i_min; i++)
			{
				i_max--;
				a_mcl[i_max].w() /= 2;
				a_mcl[i] = a_mcl[i_max];
			}
		} //}}}
	};
}

#endif // DRE_H_INCLUDED

