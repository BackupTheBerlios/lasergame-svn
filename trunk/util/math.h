#ifndef UTIL_MATH_H_INCLUDED
#define UTIL_MATH_H_INCLUDED 1

#include <float.h>
#include <math.h>

#ifdef _WIN32
	
	inline double copysign (double x, double y) { return _copysign(x,y); }
	inline double chgsign (double x) { return _chgsign(x); }
	inline double scalb(double x, long y){ return _scalb(x,y); }
//	inline double logb(double x){ return _logb(x); }
	inline double nextafter(double x, double y){ return _nextafter(x, y); }
	inline int    finite(double x){ return _finite(x); }
	inline int    isnan(double x){ return _isnan(x); }
	inline int    fpclass(double x){ return _fpclass(x); }

# ifndef M_PI
#		define M_PI 3.1415926535897932384626433832795
#endif

#	ifdef M_SQRT2
#		define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
#	endif

	
#elif defined linux

#endif

#endif //MY_MATH_H_INCLUDED


