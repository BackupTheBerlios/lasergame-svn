#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED 1

/** @file
*         %Time definition.
*         
* @author Zbynek Winkler (c) 2003, 2004 <zw at robotika cz>
*
* $Id$
*/


//#include "number.h"
#include <stdint.h>

namespace num
{
/// Time encapsulating class
class Time //{{{1
{
	int32_t m_time; ///< The time stored
protected:
	/// Just stores in_msec to m_data
	Time(const int32_t & in_time) : m_time(in_time) {}
public:
	/// Default initialization (initializes to zero time)
	Time() : m_time(0) {}
	/// Returns time in miliseconds
	//! OP: it is guaranted to be rounded DOWN
	int ms() const //{{{2
	{ 
		//return m_time*256/366000;
		return (int)(int64_t(m_time)*16/22875);
	}
	//{{{2 comparision ops
	friend bool operator < (const Time& a, const Time& b)
	{
		return a.m_time < b.m_time;
	}
	bool lt(const Time& a = Time())
	{
		return *this < a;
	}
	friend bool operator > (const Time& a, const Time& b)
	{
		return a.m_time > b.m_time;
	}
	bool gt(const Time& a = Time())
	{
		return *this > a;
	}
	friend bool operator <= (const Time& a, const Time& b)
	{
		return a.m_time <= b.m_time;
	}
	friend bool operator >= (const Time& a, const Time& b)
	{
		return a.m_time >= b.m_time;
	}
	friend bool operator == (const Time& a, const Time& b)
	{
		return a.m_time == b.m_time;
	}
	friend bool operator != (const Time& a, const Time& b)
	{
		return a.m_time != b.m_time;
	}
	//{{{2 arithmetic ops
	friend Time operator - (const Time& a, const Time& b)
	{
		return Time(a.m_time - b.m_time);
	}	
	friend Time operator + (const Time& a, const Time& b)
	{
		return Time(a.m_time + b.m_time);
	}
	Time operator * (const int& a_num) const
	{
		return Time(m_time * a_num);
	}
	Time& operator += (const Time& a_time)
	{
		m_time += a_time.m_time;
		return *this;
	}
	Time& operator -= (const Time& a_time)
	{
		m_time -= a_time.m_time;
		return *this;
	}
	//}}}
};

/// Initializes Time from miliseconds
class MSec : public Time //{{{1
{
public:
	/// Passes the in_msec to Time(const int32_t&)
	MSec(const int & in_msec) : Time(in_msec*22876/16) {}
};

/// Initializes Time from seconds
class Sec : public MSec //{{{1
{
	public:
		/// Passes the in_msec to Time(const int32_t&)
		Sec(const int & in_sec) : MSec(in_sec*1000) { }
};

///
class SysTime : public Time //{{{1
{
public:
	SysTime(const uint32_t in_time) : Time(in_time&0x7fffffff) { }
};
//}}}

#if 0
Number operator / (const Time& a_time) const
	{
		return Number(m_time, a_time.m_time);
	}
#endif
}

#endif

