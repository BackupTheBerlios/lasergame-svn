#ifndef POSE_H_INCLUDED
#define POSE_H_INCLUDED 1

/** @file
* 
*         %Point and %Pose definitions.
*         
* @author Zbynek Winkler (c) 2004 <zw at robotika cz>
*
* $Id$
*/

#include <ostream>
#include <iomanip>
#include <ios>
#include "dist.h"
#include "angle.h"
#include "rnd.h"

#ifndef DOXYGEN_SKIP
#define DOXYGEN_SKIP(X) X
#endif

namespace num {

template <class T> class PointT //{{{1
{
public:
	/// Constructor from 2 coordinates
	PointT(const DistT<T> & in_x, const DistT<T> & in_y)	: m_x(in_x), m_y(in_y)	{}

	/// Does nothing (DistT<T> set themselves to zero)
	PointT() {}

	/// @name Accessors
	/// @{
	const DistT<T> & x() const { return m_x; }
	const DistT<T> & y() const { return m_y; }
	DistT<T> & x() { return m_x; }
	DistT<T> & y() { return m_y; }
	/// @}
	
	struct IntOutput //{{{2 
	{
		const PointT<T>& m_p;
		IntOutput(const PointT<T>& p) : m_p(p) {}
	};
	IntOutput n() const { return IntOutput(*this); }

	/// Equality operator uses equality operators of dist
	bool operator == (const PointT & in_pos) const //{{{2
	{
		return in_pos.m_x == m_x && in_pos.m_y == m_y;
	}
	/// Equality with epsilon (box type)
	bool eq(const PointT& in_PointT = PointT(), const DistT<T>& eps = DistT<T>::EPSILON()) const //{{{2
	{
		return m_x.eq(in_PointT.m_x, eps) && m_y.eq(in_PointT.m_y, eps);
	}
	//{{{2
	/// @name Vector arithmetic
	/// @{
	PointT operator - () const { return PointT(Milim(0)-m_x, Milim(0)-m_y); }
	PointT operator - (const PointT& in) const { return PointT(m_x - in.m_x, m_y - in.m_y); }
	PointT operator + (const PointT& in) const { return PointT(m_x + in.m_x, m_y + in.m_y); }
	PointT & operator += (const PointT& in) { m_x += in.m_x; m_y += in.m_y; return *this;	}
	/// @}

	//{{{2
	/// @name Vector operations
	/// @{
	/// DistT<T>ance of the PointT from the origin (also length of the vector)
	DistT<T> magnitude() const { return DistT<T>::hypot(m_x, m_y); }
	/// DistT<T>ance to other PointT
	DistT<T> distanceTo(const PointT &p) const { return (p - *this).magnitude(); }
	/// %Angle if represented in polar coordinates
	operator Angle () const { return Angle::atan2(m_y.m(), m_x.m()); }
	/// Distance if represented in polar coordinates
	operator DistT<T> () const { return magnitude(); }
	/// @}
	
	/// Return true if p lies in the box specified by box{1|2} points (inclusive)
	static inline bool inBox(const PointT &p, const PointT &box1, const PointT &box2) //{{{2
	{
		return ( p.y() <= box1.y() || p.y() <= box2.y() ) && ( p.y() >= box1.y() || p.y() >= box2.y() )
		    && ( p.x() <= box1.x() || p.x() <= box2.x() ) && ( p.x() >= box1.x() || p.x() >= box2.x() );
	}
	
protected:
	DistT<T> m_x; ///< x coordinate
	DistT<T> m_y; ///< y coordinate
}; 
//}}}1
/// Position encapsulating class
template <class T> class PoseT //{{{1
{
	PointT<T> m_point; ///< origin 
  Angle m_heading; ///< orientation
public:
	/// Initializes from supplied values
	PoseT(const DistT<T> & in_x, const DistT<T> & in_y, const Angle & in_heading) 
		: m_point(in_x, in_y), m_heading(in_heading) { }
	/// Initializes from a place and a heading
	PoseT(const PointT<T> &in_p, const Angle & in_heading) 
		: m_point(in_p), m_heading(in_heading)	{ }

	/// Does nothing (memebers set themselves to zero)
	PoseT() {}

	/// Equality operator uses equality operators of Point and Angle
	bool operator == (const PoseT & in_pos) const //{{{2
	{
		return in_pos.m_point == this->m_point && in_pos.m_heading == m_heading;
	}
	/// Equality with epsilon (which is PoseT also)
	bool eq(const PoseT& in_p = PoseT(), const PoseT& eps = PoseT::EPSILON()) const //{{{2
	{
		return m_point.eq(in_p.m_point, eps.m_point) && m_heading.eq(in_p.m_heading, eps.m_heading);
	}

	static inline PoseT EPSILON() //{{{2
	{
		return PoseT(DistT<T>::EPSILON(), DistT<T>::EPSILON(), Angle::EPSILON());
	}
	//{{{2
	/// @name Accessors
	/// @{
	const Angle & heading() const { return m_heading; }
	const DistT<T> & x() const { return m_point.x(); }
	const DistT<T> & y() const { return m_point.y(); }
	Angle & heading() { return m_heading; }
	DistT<T> & x() { return m_point.x(); }
	DistT<T> & y() { return m_point.y(); }
	/// @}

	struct IntOutput //{{{2
	{
		const PoseT& m_p;
		IntOutput(const PoseT& p) : m_p(p) {}
	};
	IntOutput n() const { return IntOutput(*this); }

	/// Generic setter for all values
	void set(const DistT<T> & in_x, const DistT<T> & in_y, const Angle & in_heading) //{{{2
	{
		m_point.x() = in_x;
		m_point.y() = in_y;
		m_heading = in_heading;
	}

	/// Init from in_pos with a supplied noise
	void set(const PoseT& in_pos, Rnd & in_rnd, const DistT<T>& in_posNoise, const Angle& in_angleNoise) //{{{2
	{
		m_point.x() = in_pos.x() + in_posNoise * in_rnd();
		m_point.y() = in_pos.y() + in_posNoise * in_rnd();
		m_heading = in_pos.m_heading + in_angleNoise * in_rnd();
	}

	/// Move the position along a stored heading by the supplied distance
	PoseT & advanceBy(const DistT<T> & in_f) //{{{2
	{
		m_point.x() += in_f * m_heading.cos();
		m_point.y() += in_f * m_heading.sin();
		return *this;
	}

	/// Move the position perpendicular to the heading by the supplied distance (positive values to the right)
	PoseT & slideBy(const DistT<T> & in_s) //{{{2
	{
		m_point.x() -= in_s * m_heading.sin();
		m_point.y() += in_s * m_heading.cos();
		return *this;
	}

	/// Add to the stored heading
	PoseT & turnBy(const Angle & in_angle) //{{{2
	{
		m_heading += in_angle;
		return *this;
	}
	
	/// Advance by f and slide by s
	PoseT & offsetBy(const DistT<T> & in_f, const DistT<T> & in_s) //{{{2
	{
		this->advanceBy(in_f);
		this->slideBy(in_s);
		return *this;
	}

	/// Advance by f and slide by s
	PoseT & offsetBy(const PointT<T> & in_off) //{{{2
	{
		return this->offsetBy(in_off.x(), in_off.y());
	}

	/// Calculate offset to be added in order to move to the specified place
	PointT<T> offsetTo(const PointT<T> &p) const  //{{{2
	{ 
		PointT<T> pom(p - this->m_point);
		Angle a = Angle(pom) - m_heading;
		DistT<T> d = DistT<T>(pom);
		return PointT<T>(d * a.cos(), d * a.sin()); 
	}

public:
	/// Returns distance of point from line defined by (this) position
	DistT<T> lineToPointDistance(const PointT<T> &in_point) const //{{{2
	{
/*	Place diff = point - (const Place &)*this;
		Angle zero_to_point = Angle::atan2(diff.x(), diff.y());
		return diff.magnitude() * (Deg(90) - zero_to_point - heading()).sin();
		*/
		PointT<T> diff = in_point - this->m_point;
		Angle zero_to_point = Angle::atan2(diff.x(), diff.y());
		Angle line_to_point = (zero_to_point - heading());
		line_to_point.normalize();
		if(line_to_point < Deg(0)) line_to_point.oppositeAngle();
		ASSERT(line_to_point.sin() >= 0.0);
		return diff.magnitude() * line_to_point.sin();
	}

private: //{{{2
//}}}1
};

template <class T> inline std::ostream& operator << (std::ostream & o, const PoseT<T>& p) //{{{1
{
	return o << std::setw(10) << std::left << p.x().m() 
	         << std::setw(10) << std::left << p.y().m()
	         << std::setw(10) << std::left << p.heading().rad();
}

template <class T> inline std::ostream& operator << (std::ostream & o, const PointT<T>& p) //{{{1
{
	return o << std::setw(10) << std::left << p.x().m() 
	         << std::setw(10) << std::left << p.y().m();
	//return o << setfill(' ') << right << setw(5) << p.x().mm() << ", " << setw(5) << p.y().mm();
}

template <class T> inline std::ostream& operator << (std::ostream & o, const typename PointT<T>::IntOutput& ip) //{{{1
{
	using namespace std;
	const PointT<T>& p(ip.m_p);
	return o << setfill(' ') << right << setw(5) << p.x().mm() << ", " << setw(5) << p.y().mm();
}

template <class T> inline std::ostream& operator << (std::ostream & o, const typename PoseT<T>::IntOutput& ip) //{{{1
{
	return o << ip.m_p->m_point << ", " << std::setw(4) << ip.m_p.heading().deg();
}

//{{{1 cmp{X,Y,Heading} ///////////////////////////////////////
template <class T> inline bool cmpX(const PoseT<T> & in_a, const PoseT<T> & in_b)  //{{{2
{ 
	return in_a.x() < in_b.x(); 
}
template <class T> inline bool cmpY(const PoseT<T> & in_a, const PoseT<T> & in_b)  //{{{2
{ 
	return in_a.y() < in_b.y(); 
}
template <class T> inline bool cmpHeading(const PoseT<T> & in_a, const PoseT<T> & in_b)  //{{{2
{ 
	return in_a.heading() < in_b.heading(); 
}
//}}}
//}}}

typedef PointT<Dist::type> Point;
typedef PoseT<Dist::type> Pose;

} // namespace num

#endif // POSE_H_INCLUDED

