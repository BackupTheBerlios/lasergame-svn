#ifndef ANGLE_H_INCLUDED
#define ANGLE_H_INCLUDED 1

#include "util/math.h"

namespace num
{
	struct inRads //{{{1
	{
		static const double DEG_MULT() { return M_PI/180.0; } // 360 = 2*pi -> 1 = 2*pi/360 = pi/180
		static const int RAD_MULT() { return 1; }
	};
	
	struct inMRads //{{{1
	{
		static const double DEG_MULT() { return M_PI*1000/180; }
		static const int RAD_MULT() { return 1000; }
	}; 
	
	struct inMDegs //{{{1
	{
		static const int DEG_MULT() { return 1000; }
		static const double RAD_MULT() { return 180*1000/M_PI; }
	}; 
	
	/// Angle encapsulating class
	template <class T, class P> class AngleT //{{{1
	{
		T m_data;
		mutable double m_sin;
		mutable double m_cos;
		bool m_valid; ///< Are m_sin and m_cos valid?
	protected:
		/// Just copies the param to m_data
		AngleT(const T & in_angle) : m_data(in_angle), m_valid(false) { }
	
	public:
		/// @name Comparision operators 
		/// @{
		bool operator == (const AngleT& in_num) const { return this->m_data == in_num.m_data; }
		bool operator >  (const AngleT& in_num) const { return m_data >  in_num.m_data; }
		bool operator <  (const AngleT& in_num) const { return m_data <  in_num.m_data; }
		bool operator >= (const AngleT& in_num) const { return m_data >= in_num.m_data; }
		bool operator <= (const AngleT& in_num) const { return m_data <= in_num.m_data; }
		bool gt(const AngleT& in_b = AngleT(), const AngleT& eps = EPSILON()) const 
		{ 
			return *this > in_b + eps; 
		}
		bool lt(const AngleT& in_b = AngleT(), const AngleT& eps = EPSILON()) const 
		{ 
			return *this < in_b - eps; 
		}
		bool eq(const AngleT& in_b = AngleT(), const AngleT& eps = EPSILON()) const 
		{ 
			AngleT diff = *this - in_b;
			diff.normalize();
			return diff < eps && diff > -eps;
			//return !gt(in_b, eps) && !lt(in_b, eps); !!! does not work, needs normalize
		}
		/// @}
	
		/// @name Arithmetic operators
		/// @{
		AngleT operator + (const AngleT& in_num) const { return AngleT(m_data + in_num.m_data); }
		AngleT operator - (const AngleT& in_num) const { return AngleT(m_data - in_num.m_data); }
		AngleT operator * (const T& in_num) const { return AngleT(m_data * in_num); }
		AngleT operator / (const T& in_num) const { return AngleT(m_data / in_num); }
		T operator / (const AngleT& in_angle) const { return m_data / in_angle.m_data; }
		AngleT operator - () const { return AngleT(-1*m_data); }
	
		AngleT& operator += (const AngleT& in_num) { m_data += in_num.m_data; m_valid=false; return *this; }
		AngleT& operator -= (const AngleT& in_num) { m_data -= in_num.m_data; m_valid=false; return *this; }
		AngleT& operator *= (const T& in_num) { m_data *= in_num; m_valid = false; return *this; }
		AngleT& operator /= (const T& in_num) { m_data /= in_num; m_valid = false; return *this; }
	//AngleT& operator = (const AngleT& in_num) { m_data = in_num.m_data; m_valid = false; return *this; }
		/// @}
	
	public:
		typedef T type;
		/// Initializes to zero angle
		AngleT() : m_data(0), m_valid(false) { }
		/// Used when comparing for equality
		static AngleT EPSILON()	{ return AngleT(0.00001*P::RAD_MULT()); }

		/// Cosinus of the angle
		T cos() const { if (!m_valid) updateSinCos(); return m_cos; }
		/// Sinus of the angle
		T sin() const { if (!m_valid) updateSinCos(); return m_sin; }
		/// Updates the caches for sin/cos
		void updateSinCos() const 
		{
			m_sin = ::sin(rad()); 
			m_cos = ::cos(rad()); 
		}
		/// Wrapper around the system function
		static AngleT atan2(const double & y, const double & x)
		{
			return AngleT(::atan2(y, x) * P::RAD_MULT());
		}
		
		double rad() const { return toDouble(m_data/P::RAD_MULT()); }
		int deg() const { return roundToInt(m_data,1000)/roundToInt(P::DEG_MULT(),1000); }
		
		AngleT& normalize()
		{
			while(m_data >  180*P::DEG_MULT()) m_data -= 360*P::DEG_MULT();
			while(m_data < -180*P::DEG_MULT()) m_data += 360*P::DEG_MULT();
			return *this;
		}
	};

	/// Initializes Angle from radians
	template <class T, class P> class RadT : public AngleT<T,P> //{{{1
	{
	public:
		/// Constructs angle from radians.
		RadT(const T & in_rad) : AngleT<T,P>(in_rad*P::RAD_MULT()) { }
		/// Default initialization (initializes to zero angle)
		RadT() { }
	};
	
	/// Initializes Angle from degrees
	template <class T, class P> class DegT : public AngleT<T,P> //{{{1
	{
	public:
		/// Converts degrees to radians to be stored in Angle
		DegT(const int & in_deg) : AngleT<T,P>(in_deg*P::DEG_MULT()) { }
		/// Default initialization (initializes to zero angle)
		DegT() { }
	};
	//}}}

	typedef RadT<double, inRads> Rad;
	typedef DegT<double, inRads> Deg;
	typedef AngleT<double, inRads> Angle;

} // namespace num

#endif // ANGLE_H_INCLUDED

