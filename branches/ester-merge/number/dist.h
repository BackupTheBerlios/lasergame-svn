#ifndef NUMBER_DIST_H_INCLUDED
#define NUMBER_DIST_H_INCLUDED 1

/** @file
*         %Dist, Milim and Meter definition.
*         
* @author Zbynek Winkler (c) 2003, 2004 <zw at robotika cz>
*
* $Id$
*/

#include <ostream>
#include "number.h"

namespace num
{
	/// Distance encapsulating class
	template <typename T> class DistT //{{{1
	{
	protected:
		/// The distance stored
		T m_data;
		/// Just copies the param to m_data
		explicit DistT(const T & in_num) : m_data(in_num) { }
	public:
		typedef T type;
		/// Initializes to zero distance
		DistT() : m_data(0) { }
		//operator T() const { return m_data; }
		/// Used when comparing for equality
		static DistT EPSILON();
		/// Wrapper around the system function
		static DistT hypot(const DistT& a, const DistT& b)
		{
			return DistT(::hypot(toDouble(a), toDouble(b)));
		}
		/// Wrapper around the system function
		static DistT sqrt(const DistT& a)
		{
			return DistT(::sqrt(toDouble(a)));
		}
	
		/// @name Comparision operators
		/// @{
		bool operator == (const DistT& in_num) const { return this->m_data == in_num.m_data; }
		bool operator >  (const DistT& in_num) const { return m_data >  in_num.m_data; }
		bool operator <  (const DistT& in_num) const { return m_data <  in_num.m_data; }
		bool operator >= (const DistT& in_num) const { return m_data >= in_num.m_data; }
		bool operator <= (const DistT& in_num) const { return m_data <= in_num.m_data; }
		bool gt(const DistT & dist = DistT(), const DistT & eps = EPSILON()) const 
		{ 
			return *this > dist + eps; 
		}
		bool lt(const DistT & dist = DistT(), const DistT & eps = EPSILON()) const 
		{ 
			return *this < dist - eps; 
		}
		bool eq(const DistT & in_b = DistT(), const DistT & eps = EPSILON()) const 
		{ 
			return !gt(in_b, eps) && !lt(in_b, eps); 
		}
		/// @}
	
		/// @name Arithmetic operators
		/// @{
		DistT operator + (const DistT& in_dist) const  { return DistT(m_data + in_dist.m_data); }
		DistT operator - (const DistT& in_dist) const  { return DistT(m_data - in_dist.m_data); }
		DistT operator - () const                      { return DistT(T(0) - m_data); }
		DistT operator * (const T& in_num) const       { return DistT(m_data * in_num); }
		DistT operator * (const int& in_num) const     { return DistT(in_num * m_data); }
		DistT operator / (const T& in_num) const       { return DistT(m_data / in_num); }
		DistT operator / (int in_num) const            { return DistT(m_data / in_num); }
		T     operator / (const DistT& in_dist) const  { return m_data / in_dist.m_data; }
	
		DistT& operator += (const DistT& in_num) { m_data += in_num.m_data; return *this; }
		DistT& operator -= (const DistT& in_num) { m_data -= in_num.m_data; return *this; }
		DistT& operator *= (const T& in_num)     { m_data *= in_num; return *this; }
		DistT& operator /= (const T& in_num)     { m_data /= in_num; return *this; }
		/// @}
		
		template <typename TT> friend double toDouble(const DistT<TT> & in);
	
		/// Returns the distance in milimeters
		int mm() const 
		{ 
			return roundToInt(m_data, 1000);
		}
		/// Returns the distance in meters
		double m() const { return toDouble(m_data); }
	
		///^2
		T power() const { return m_data * m_data; };
	};
	
	/// Initializes DistT from milimeters
	template <typename T> class MilimT : public DistT<T> //{{{1
	{
	public:
		/// Converts in_mm to the storage format DistT requires
		MilimT(const T & in_mm) : DistT<T>(in_mm/1000) { }
		/// Converts in_mm to the storage format DistT requires
		MilimT(int in_mm) :  DistT<T>(make<T>(in_mm, 1000)) { }
		/// Default initialization (initializes to zero distance)
		MilimT() { }
	};

	/// Initializes DistT from meters
	template <typename T> class MeterT : public DistT<T> //{{{1
	{
	public:
		/// Converts in_m to the storage format DistT requires
		MeterT(const T & in_m) : DistT<T>(in_m) { }
		/// Default initialization (initializes to zero distance)
		MeterT() { }
	};
	
	//{{{1 DistT friend and static inlines
	template <typename T> inline DistT<T> DistT<T>::EPSILON() { return MilimT<T>(make<T>(2,10)); }
	template <typename T> inline double toDouble(const DistT<T> & in) { return toDouble(in.m_data); }
	template <typename T> inline std::ostream& operator << (std::ostream & o, const DistT<T> & d) 
	{ return o << d.m(); }
	//}}}
	typedef DistT<double> Dist;
	typedef MilimT<double> Milim;
	typedef MeterT<double> Meter;
}

#endif // DIST_H_INCLUDED

