#ifndef _POSE3D_H_
#define _POSE3D_H_

#include "number/dist.h"


namespace num3D
{
	
using namespace num;

template <typename T> class Point3DT
{
	public:
		Point3DT() {}

		Point3DT(const DistT<T> & in_x, const DistT<T> & in_y, const DistT<T> & in_z) : 
			m_x(in_x), m_y(in_y), m_z(in_z) {}

		Point3DT(const DistT<T> & in_x, const DistT<T> & in_y) :
			m_x(in_x), m_y(in_y) {}
		/// @name Accessors
	  /// @{
		const DistT<T> & x() const {return m_x;}
		const DistT<T> & y() const {return m_y;}
		const DistT<T> & z() const {return m_z;}

		DistT<T> & x() {return m_x;}
		DistT<T> & y() {return m_y;}
		DistT<T> & z() {return m_z;}
		/// @}
	protected:
	  num::Dist<T> m_x, m_y, m_z;
};

class Quaternion
{
	public:
		Quaternion() : m_u(1), m_x(0), m_y(0), m_z(0) {}
		Quaternion(double u, double x, double y, double z) : 
			m_u(u), m_x(x), m_y(y), m_z(z) {}
		const double & u() const  { return m_u; }
		const double & x() const  { return m_x; }
		const double & y() const  { return m_y; }
		const double & z() const  { return m_z; }
		double & u() { return m_u; }
		double & x() { return m_x; }
		double & y() { return m_y; }
		double & z() { return m_z; }

	protected:
		double m_u, m_x, m_y, m_z;
};

template <typename T> class Pose3DT
{
	public:
		Pose3DT() {}
		Pose3DT(const DistT<T> & in_x, const DistT<T> & in_y) : m_point(in_x, in_y) {}
		Point3DT<T> & point() { return m_point; }
		Quaternion & quaternion() { return m_quaternion; }
		const Point3DT<T> & point() const  { return m_point; }
		const Quaternion & quaternion() const  { return m_quaternion; }

		
	protected:
		Point3DT<T> m_point;
		Quaternion m_quaternion;

};

typedef Point3DT<Dist::type> Point3D;
typedef Pose3DT<Dist::type> Pose3D;
		
};

#endif
