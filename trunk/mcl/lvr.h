#ifndef LVR_H
#define LVR_H

/** @file
*              LVR declaration
*
* @author Zbynek Winkler (c) 2003 <zw at robotika cz>
*
* $Id$
*/

#include <algorithm>
#include "mcl.h"

namespace mcl
{
	class LVR
	{
		typedef double Number;
		std::vector<Number> m_cumulativeSum;
		std::vector<Sample> m_temp;
		struct add_weight
		{
			Number m_sum;
			add_weight() : m_sum(0) {}
			Number operator () (const Sample & s) { return m_sum += s.w(); }
		};
	public:
		void operator () (MCL & a_mcl, num::Rnd & a_rnd) //{{{
		{
			using namespace std;
			m_cumulativeSum.resize(a_mcl.size());
			transform(a_mcl.begin(), a_mcl.end(), m_cumulativeSum.begin(), add_weight());
			ASSERT( m_cumulativeSum.size() == a_mcl.size() );
			// average weight is 1
			Number averageWeight = 1; //Number(1) >> a_mcl.scale();
			// choose random starting position for low variance walk
			// random number from (0,1)
			Number currentWeight = (a_rnd()+1)/2;
			ASSERT( currentWeight != 1 );
			// scale down as the cs is
			currentWeight = currentWeight;// >> a_mcl.scale();
			MCL::iterator s = a_mcl.begin();
			size_t i = 0;
			while (m_temp.size() < a_mcl.size())
			{
				while(m_cumulativeSum[i] < currentWeight) i++;
				ASSERT( i < a_mcl.size() );
				m_temp.push_back(s[i]);
				m_temp.back().w() = 1;
				currentWeight += averageWeight;
			}
			a_mcl.swap(m_temp);
			m_cumulativeSum.clear();
			m_temp.clear();
		} //}}}
	};
}

#endif // LVR_H

