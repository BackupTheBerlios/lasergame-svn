#ifndef MCL_H_INCLUDED
#define MCL_H_INCLUDED 1

/** @file
*              Implementation of MCL
*
* @author Zbynek Winkler (c) 2003, 2004 <zw at robotika cz>
*
* $Id$
*/
#include <ostream>
#include <vector>
#include "number/Pose.h"

namespace mcl 
{
/// Position of the robot
class Sample : public num::Pose //{{{1
{
	double m_w;  ///< weight
	int m_p;     ///< parent
public:
	/// Default weight set to one
	Sample() : m_w(1) { }
	/// Parent access
	int & parent() { return m_p; }
	/// Weight access
	double & w() { return m_w; }
	/// Weight access
	const double & w() const { return m_w; }
	/// Output x, y and weight separated by tabs
	friend inline std::ostream& operator << (std::ostream& out, const Sample& what)
	{
		return out << *static_cast<const num::Pose*>(&what) << '\t' << what.m_w;
	}
};

/// Compare two samples by their weights
inline bool cmpWeight(const Sample & in_a, const Sample & in_b) 
{ 
	return in_a.w() < in_b.w(); 
}

/// Implementation of the MCL algorithm (without resampling)
class MCL : public std::vector<Sample> //{{{1
{
	int m_scale;              ///< Log of the number of samples
	
public:
	MCL(int in_numsamp);      ///< Allocates memory
	/// Init the distribution at a_pos with a supplied noise
	void init(const num::Pose & in_pos, num::Rnd & in_rnd, const num::Dist & in_posNoise=num::Milim(20), 
			const num::Angle & in_angleNoise=num::Deg(10));
	/// Returns averaged sample
	void getResult(num::Pose & out);
	/// Returns averaged sample
	num::Pose getResult() { num::Pose ret; getResult(ret); return ret; }
	/// Normalize the weights of the samples so that sum(w_i) == numSamples
	void normalize();
	/// Returns m_scale
	int scale() const { return m_scale; }
	friend std::ostream& operator << (std::ostream& out, const MCL& what);
};
//}}}
}

#endif // MCL_H_INCLUDED

