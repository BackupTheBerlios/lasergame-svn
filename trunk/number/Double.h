/**
* @file
* Class implementing floating point arithmetics. Mainly just wrapper around double.
*
* @author Jaroslav Sladek & Zbynek Winkler
*/

#ifndef DOUBLE_H_INCLUDED 
#define DOUBLE_H_INCLUDED 1

#include <stdint.h>
#include "util/math.h"

namespace num {

class Double
{
	public:
		Double() {}
		Double(double in_value) : m_value(in_value) {}
		Double(double in_value, int in_value2) : m_value(in_value/in_value2) {}
		static double epsilon() { return 1e-8; }
		bool isValid() { return !isnan(m_value) && finite(m_value); }
		friend Double operator+(const Double& in_a, const Double& in_b);
		friend Double operator-(const Double& in_a, const Double& in_b);
		friend Double operator*(const Double& in_a, const Double& in_b);
		friend Double operator/(const Double& in_a, const Double& in_b);
		Double& operator+=(const Double& in_num)
		{
			m_value += in_num.m_value;
			return *this;
		}
		Double& operator-=(const Double& in_num)
		{
			m_value -= in_num.m_value;
			return *this;
		}
		Double& operator*=(const Double& in_num)
		{
			m_value *= in_num.m_value;
			return *this;
		}
		Double& operator/=(const Double& in_num)
		{
			m_value /= in_num.m_value;
			return *this;
		}
		Double operator>>(const int i) const
		{
			return Double(m_value/(1<<i));
		}
		Double& operator>>=(const int i)
		{
			m_value = m_value/(1<<i);
			return *this;
		}
		Double operator<<(const int i) const
		{
			return Double(m_value*(1<<i));
		}
		Double& operator<<=(const int i)
		{
			m_value = m_value*(1<<i);
			return *this;
		}
		friend bool operator==(const Double& in_a, const Double& in_b);
		bool eq(const Double& in_b)
		{
			double diff = m_value - in_b.m_value;
			return -Double::epsilon() < diff && diff < Double::epsilon();
		}
		friend bool operator!=(const Double& in_a, const Double& in_b);
		friend bool operator<=(const Double& in_a, const Double& in_b);
		friend bool operator>=(const Double& in_a, const Double& in_b);
		friend bool operator<(const Double& in_a, const Double& in_b);
		friend bool operator>(const Double& in_a, const Double& in_b);
		friend double toDouble(const Double& in);
		friend int toInt(const Double& in);
		friend int toInt(const Double& in, int mult);
		friend int roundToInt(const Double& in);
		friend int roundToInt(const Double& in, int mult);
		friend int32_t toValue(const Double& in);
	private:
		double m_value;
};

inline Double operator+(const Double& in_a, const Double& in_b)
{
	return Double(in_a.m_value + in_b.m_value);
}

inline Double operator-(const Double& in_a, const Double& in_b)
{
	return Double(in_a.m_value - in_b.m_value);
}

inline Double operator*(const Double& in_a, const Double& in_b)
{
	return Double(in_a.m_value * in_b.m_value);
}

inline Double operator/(const Double& in_a, const Double& in_b)
{
	return Double(in_a.m_value / in_b.m_value);
}

inline bool operator==(const Double& in_a, const Double& in_b) 
{ 
	return in_a.m_value == in_b.m_value;
}

inline bool operator!=(const Double& in_a, const Double& in_b) 
{ 
	return in_a.m_value != in_b.m_value;
}

inline bool operator<=(const Double& in_a, const Double& in_b) 
{ 
	return in_a.m_value <= in_b.m_value;
}

inline bool operator>=(const Double& in_a, const Double& in_b) 
{ 
	return in_a.m_value >= in_b.m_value;
}

inline bool operator<(const Double& in_a, const Double& in_b) 
{ 
	return in_a.m_value < in_b.m_value;
}

inline bool operator>(const Double& in_a, const Double& in_b) 
{ 
	return in_a.m_value > in_b.m_value;
}

inline double toDouble(const Double& in) { return in.m_value; }
namespace	{
	inline int round_to_int(double d)	{ return d > 0.0 ? (int) (d + 0.5) :  (int) (d - 0.5); }
}

inline int roundToInt(const Double& in) 	{ return round_to_int(in.m_value); }
inline int roundToInt(const Double& in, int mult) 	{ return round_to_int(in.m_value * mult); }

///floors!
inline int toInt(const Double& in) { return int(in.m_value); }
inline int toInt(const Double& in, int mult) { return int(in.m_value * mult); }
// Return underlying representation. Not for normal use, since implementation can change
//inline int32_t toValue(const Double& in) { return toValue(FixedPoint(in.m_value)); }

} // namespace num

#endif

