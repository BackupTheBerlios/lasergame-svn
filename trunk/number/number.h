#ifndef NUMBER_NUMBER_H_INCLUDED
#define NUMBER_NUMBER_H_INCLUDED 1

/** @file
*         Various support functions.
*         
* @author Zbynek Winkler (c) 2004 <zw at robotika cz>
*
* $Id$
*/

namespace num {
	template <class T> inline T make(const int& a, const int& b) { return T(a)/T(b); }
	inline double toDouble(const double & a) { return a; }
}

#endif

