#include "mcl.h"
#include "util/assert.h"

/** @file
*              Implementation of MCL
*
* @author Zbynek Winkler (c) 2003, 2004 <zw at robotika cz>
*
* $Id$
*/

using num::Pose;
using num::Dist;
using num::Angle;

namespace mcl
{
MCL::MCL(int in_numsamp) //{{{1 ////////////////////////////////////
{ 
	ASSERT( in_numsamp > 0 );
	resize(in_numsamp);
	for (m_scale = -1; in_numsamp != 0; m_scale++) in_numsamp >>= 1;
}
// }}}
//{{{1 MCL::init ///////////////////////////////////////////////////
void MCL::init(const Pose & in_pos, num::Rnd & in_rnd, const Dist & in_posNoise, 
		const Angle & in_angleNoise)
{
	for (MCL::iterator s = begin(); s != end(); s++)
	{
		s->set(in_pos, in_rnd, in_posNoise, in_angleNoise);
		s->parent() = s - begin();
	}
}

void MCL::getResult(Pose & out) //{{{1
{
	Dist x, y;
	double w = 0, ax = 0, ay = 0;

	for (MCL::iterator s = begin(); s != end(); s++)
	{
		x += s->x() * s->w();
		y += s->y() * s->w();
		ax += s->heading().cos() * s->w();
		ay += s->heading().sin() * s->w();
		w += s->w();
	}

	out.set(x/w, y/w, Angle::atan2(ay, ax));
	//out.w() = w/size();
}

void MCL::normalize() //{{{1 ///////////////////////////////////////
{
	double gain = 0, w = 0;
	MCL::iterator s;
	for (s = begin(); s != end(); s++)
		w += s->w();

	gain = size() / w;
	ASSERT( num::isValid(gain) );
	for (s = begin(); s != end(); s++)
		s->w() *= gain;
}
//{{{1 MCL::operator << ////////////////////////////////////////////
std::ostream& operator << (std::ostream& out, const MCL& what)
{
	MCL::const_iterator s;
	for (s = what.begin(); s != what.end(); s++)
		out << *s << std::endl;
	return out;
}
///////////////////////////////////////////////////////////}}}
}
