#ifndef MEASURES_H_INCLUDED
#define MEASURES_H_INCLUDED 1

/** @file
* 
*         %Basic field measures
*         
* @author Zbynek Winkler, Martin Dlouhy, Ondrej Pacovsky, Jaroslav Sladek
*
* $Id$
*/
	
#include "number/dist.h"
#include "number/pose.h"

namespace measures 
{
  using namespace num;
	inline Dist TILE() { return Milim(300); }
	inline Dist SIZE_X() { return Milim(2400); }
	inline Dist SIZE_Y() { return Milim(2100); }
	inline Pose INITIAL() { return Pose(Milim(120), SIZE_Y() - TILE(), Deg(0)); }

	inline bool isInsideField(const Point& in_p) 
	{ 
		return in_p.x() >= Dist() && in_p.x() <= SIZE_X()  && 
		       in_p.y() >= Dist() && in_p.y() <= SIZE_Y(); 
	}
	/// Floor color
	enum Color { WHITE = 0, BLACK = 1, GREY = 2 };
	/// Returns color of the field at the suplied position
	static inline Color getColor(const Point & in_p, int invert)
	{
		int white = ((toInt(in_p.x()/TILE()) ^ toInt(in_p.y()/TILE())) & 1) ^ (invert & 1);
		return white ? WHITE : BLACK;
	}
};

#endif

