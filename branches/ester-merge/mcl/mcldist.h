#ifndef MCLDIST_H_INCLUDED
#define MCLDIST_H_INCLUDED 1

/**
* @file
* @brief Compute a difference of two distributions
*
* @author Zbynek Winkler (c) 2004 <zw at robotika cz>
*
* $Id$
*/

#include <ostream>
#include <algorithm>
#include <vector>
#include <numeric>
#include "mcl.h"

namespace mcl
{
	namespace MCLDist
	{
	//{{{1 min/max ////////////////////////////////////////////////
		/// Return the smaller of a and b
		template<class A> const A& min(const A& a, const A& b) { return a < b ? a : b; }
		/// Return the larger of a and b
		template<class A> const A& max(const A& a, const A& b) { return a > b ? a : b; }
	//{{{1 addWeight //////////////////////////////////////////////
	struct AddWeight
	{
		Sample operator() (Sample acc, Sample s)
		{
			s.w() += acc.w();
			return s;
		}
	};
	//{{{1 mclDist ////////////////////////////////////////////////
	inline double mclDist(MCL & in_mcl1, MCL & in_mcl2)
	{
		using namespace std;
		//{{{2 copy the sample sets to a buffer
		vector<Sample> x1(in_mcl1.begin(), in_mcl1.end());
		vector<Sample> y1(in_mcl1.begin(), in_mcl1.end());
		vector<Sample> x2(in_mcl2.begin(), in_mcl2.end());
		vector<Sample> y2(in_mcl2.begin(), in_mcl2.end());
	
		//{{{2 sort the samples accordingly
		sort(x1.begin(), x1.end(), num::cmpX<Sample::type>);
		sort(y1.begin(), y1.end(), num::cmpY<Sample::type>);
		sort(x2.begin(), x2.end(), num::cmpX<Sample::type>);
		sort(y2.begin(), y2.end(), num::cmpY<Sample::type>);
		
		//{{{2 insert biggest and smallest x,y sample 
		Sample biggest, smallest;
		biggest.w() = smallest.w() = 0;
		biggest.set(num::Meter(6), num::Meter(6), num::Deg(360));
		smallest.set(num::Meter(-6), num::Meter(-6), num::Deg(0));
		x1.insert(x1.begin(),smallest);
		y1.insert(y1.begin(),smallest);
		x2.insert(x2.begin(),smallest);
		y2.insert(y2.begin(),smallest);
		x1.push_back(biggest);
		y1.push_back(biggest);
		x2.push_back(biggest);
		y2.push_back(biggest);
		
		//{{{2 calculate the cumulative sum of weights
		partial_sum(x1.begin(), x1.end(), x1.begin(), AddWeight());
		partial_sum(y1.begin(), y1.end(), y1.begin(), AddWeight());
		partial_sum(x2.begin(), x2.end(), x2.begin(), AddWeight());
		partial_sum(y2.begin(), y2.end(), y2.begin(), AddWeight());
		
		//{{{2 algo
		//copy(x2.begin(), x2.end(), ostream_iterator<Sample>(ofstream("dist-1.dat"), "\n"));
		//copy(y2.begin(), y2.end(), ostream_iterator<Sample>(ofstream("dist-2.dat"), "\n"));
	
		double diff = 0;
	
		//ofstream f("dist-3.dat");
		MCL::iterator s = in_mcl1.begin();
		while (s != in_mcl1.end())
		{
			// find the value of the empirical distribution function for the sample s in in_mcl1
			MCL::iterator x = upper_bound(x1.begin(), x1.end(), *s, num::cmpX<Sample::type>)-1;
			MCL::iterator y = upper_bound(y1.begin(), y1.end(), *s, num::cmpY<Sample::type>)-1;
			
			ASSERT ( x != x1.end() );
			ASSERT ( y != y1.end() );
			
			double w1 = (x->w() + y->w());
			//f << *x << "     :     " << *y << endl;
			// and now in in_mcl2
			x = upper_bound(x2.begin(), x2.end(), *s, num::cmpX<Sample::type>) - 1;
			y = upper_bound(y2.begin(), y2.end(), *s, num::cmpY<Sample::type>) - 1;
	
			ASSERT ( x != x2.end() );
			ASSERT ( y != y2.end() );
	
			double w2 = (x->w() + y->w());
			//f << *x << "     :     " << *y << endl;
			// accumulate the difference
			diff = max(fabs(w1 - w2), diff);
			//f << diff << endl;
			s++;
		}
		return diff / std::distance(in_mcl1.begin(), in_mcl1.end());
	}
		//}}}
	//}}}
	};
	using MCLDist::mclDist;
}	

#endif // MCLDIST_H_INCLUDED

