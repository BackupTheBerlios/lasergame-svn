#ifndef NUMBER_SPEED_H_INCLUDED
#define NUMBER_SPEED_H_INCLUDED 1

/** @file
*        Linear and angular speed and acceleration.
*
* @author (c) Zbynek Winkler 2004 <zw at robotika cz>
* 
* $Id$
*/

#include "dist.h"
#include "angle.h"
#include "time.h"

namespace num
{
/// Linear speed
class LinearSpeed //{{{1
{
	Dist m_dist;
public:
	/// Creates speed representing Dist/sec
	explicit LinearSpeed(const Dist& a_dist) : m_dist(a_dist) {}
	explicit LinearSpeed() {}
	Dist operator * (const Time& a_time) const { return m_dist * (a_time/MSec(1000)); }
	LinearSpeed operator * (const int n) const { return LinearSpeed(m_dist*n); }
	friend bool operator > (const LinearSpeed& a, const LinearSpeed& b) { return a.m_dist > b.m_dist; }
	bool gt(const LinearSpeed& b=LinearSpeed()) const { return m_dist.gt(b.m_dist); }
	friend bool operator < (const LinearSpeed& a, const LinearSpeed& b) { return a.m_dist < b.m_dist; }
	bool lt(const LinearSpeed& b=LinearSpeed()) const { return m_dist.lt(b.m_dist); }
	friend bool operator >= (const LinearSpeed& a, const LinearSpeed& b) { return a.m_dist >= b.m_dist; }
	friend bool operator <= (const LinearSpeed& a, const LinearSpeed& b) { return a.m_dist <= b.m_dist; }
	friend bool operator == (const LinearSpeed& a, const LinearSpeed& b) { return a.m_dist == b.m_dist; }
	bool eq(const LinearSpeed& b=LinearSpeed()) const { return m_dist.eq(b.m_dist); }
	LinearSpeed operator - () const { return LinearSpeed(Dist() - m_dist); }
	void operator = (const Dist& d) { m_dist = d; }
	LinearSpeed& operator += (const LinearSpeed& f) { m_dist += f.m_dist; return *this; }
	LinearSpeed& operator -= (const LinearSpeed& f) { m_dist -= f.m_dist; return *this; }
	LinearSpeed& operator /= (const Dist::type& n) { m_dist /= n; return *this; }
	friend Time operator / (const Dist& d, const LinearSpeed& f);
	int mm() const { return m_dist.mm(); }
	double m() const { return m_dist.m(); }
};

inline Time operator / (const Dist& d, const LinearSpeed& f) //{{{2
{
	return Sec(d / f.m_dist); //MSec(toInt(d / f.m_dist, 1000));
}

/// Linear acceleration
class LinearAcc //{{{1
{
	Dist m_dist;
public:
	explicit LinearAcc(const Dist& d) : m_dist(d) {}
	LinearSpeed operator * (const Time& t) const { return LinearSpeed(m_dist * (t/Sec(1))); }
};

class AngularSpeed //{{{1
{
	Angle m_angle;
public:
	explicit AngularSpeed(const Angle& a_angle) : m_angle(a_angle) {}
	explicit AngularSpeed() {}
	friend bool operator > (const AngularSpeed& a, const AngularSpeed& b) { return a.m_angle > b.m_angle; }
	bool gt(const AngularSpeed& b = AngularSpeed()) { return m_angle.gt(b.m_angle); }
	friend bool operator < (const AngularSpeed& a, const AngularSpeed& b) { return a.m_angle < b.m_angle; }
	bool lt(const AngularSpeed& b = AngularSpeed()) { return m_angle.lt(b.m_angle); }
	friend bool operator == (const AngularSpeed& a, const AngularSpeed& b) { return a.m_angle == b.m_angle; }
	bool eq(const AngularSpeed& b=AngularSpeed()) { return m_angle.eq(b.m_angle); }
	AngularSpeed operator - () const { return AngularSpeed(Angle() - m_angle); }
	AngularSpeed& operator += (const AngularSpeed& a) { m_angle += a.m_angle; return *this; }
	AngularSpeed& operator -= (const AngularSpeed& a) { m_angle -= a.m_angle; return *this; }
	void operator = (const Angle& a) { m_angle = a; }
	Angle operator * (const Time& a_time) const
	{
		return m_angle * (a_time/MSec(1000));
	}
	AngularSpeed operator * (const int n) const { return AngularSpeed(m_angle*n); }
	friend Time operator / (const Angle& a, const AngularSpeed& omega) 
	{
		return Sec(a / omega.m_angle); //MSec(toInt(a / omega.m_angle, 1000));
	}
	int deg() const
	{
		return m_angle.deg();
	}
};

class AngularAcc //{{{1
{
	Angle m_angle;
	public:
		explicit AngularAcc(const Angle& a) : m_angle(a) {}
		AngularSpeed operator * (const Time& t) { return AngularSpeed(m_angle * (t/Sec(1))); }
};

class Speed //{{{1
{
	public:
		LinearSpeed m_linear;
		AngularSpeed m_angular;
};

class Acc //{{{1
{
	public:
		LinearAcc m_linear;
		AngularAcc m_angular;
};
//}}}
}

#endif

