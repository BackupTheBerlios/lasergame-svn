#ifndef RND_H_INCLUDED
#define RND_H_INCLUDED

/** @file
*         %Rnd definition.
*         
* @author Zbynek Winkler (c) 2004 <zw at robotika cz>
*
* $Id$
*/

#include <ostream>
#include <stdint.h>

#include "number.h"

namespace num {

template <class T> class RndT //{{{1
{
	uint32_t m_seed;

protected:
	uint32_t rand() { return (((m_seed = m_seed * 214013L + 2531011L) >> 16) & 0x7fff); }
	
public:
	RndT() : m_seed(1) {}
	T operator () () { return uniform(); }
	T uniform()
	{
		return make<T>(2*rand(), 0x7fff+1)-1;
	}
	double uniformD()
	{
		return 2*rand() / ((double)0x7fff+1) - 1;
	}
	void setSeed(uint32_t a_seed)
	{
		m_seed = a_seed;
	}
	template<class TT> friend std::ostream& operator << (std::ostream& out, const RndT<TT>& what);
};

typedef RndT<double> Rnd;

#if 0 //{{{1
class Normal : public Rnd
{
public:
 	// normal distribution with mean 0 and standard deviation 1
	num::Number operator () ()	{ return normal(); } 
	num::Number normal()
	{
  	double x1, x2, w;
  	do 
		{
    	//x1 = 2. * uniformD() - 1.;
    	//x2 = 2. * uniformD() - 1.;
			x1 = uniformD();
			x2 = uniformD();
    	w = x1*x1 + x2*x2;
		}
  	while (w >= 1. || w < 1E-30);
  	w = sqrt((-2.*log(w))/w);
  	x1 *= w;
  	// x2 *= w;  // a second normally distributed result not used
  	//return x1 * s + m;
		return x1;
	}
};
#endif

template <class TT> inline std::ostream& operator << (std::ostream& out, const RndT<TT>& what) //{{{1
{
	return out << what.m_seed;
}
//}}}
}
#endif //RND_H_INCLUDED

