#ifndef NUMBER_NUMBER_H_INCLUDED
#define NUMBER_NUMBER_H_INCLUDED 1

/** @file
*         Various support functions.
*         
* @author Zbynek Winkler (c) 2004 <zw at robotika cz>
*
* $Id$
*/

#include "util/math.h"

namespace num {
	template <class T> inline T make(const int& a, const int& b) { return T(a)/T(b); }
	inline double toDouble(const double & a) { return a; }
	inline int toInt(const double & a) { return int(a); }
	inline bool isValid(const double & a) { return !isnan(a) && finite(a); }
	inline bool eq(const double & a, const double & b, const double eps = 1e-4) 
	{
		double diff = a - b;
		return diff < eps && diff > -eps;
	}
	inline int roundToInt(const double &in, const int mult) 
	{ 
		return in > 0 ? int(in*mult + 0.5) : int(in*mult -0.5); 
	}
	inline int roundToInt(const double &in) 
	{ 
		return in > 0 ? int(in + 0.5) : int(in -0.5); 
	}
}

#endif

