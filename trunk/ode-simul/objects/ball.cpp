/** @file
*        Ball
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include "ball.h"
#include "../components/Body.h"
#include "../components/SphereGeom.h"

using namespace num;

void Ball::create(World* in_pWorld, num3D::Pose3D in_position)
{
	num3D::Point3D point = in_position.point();
	m_pBody = new Body(in_pWorld);
	m_pBody->setPosition(point.x().m(), point.y().m(), point.z().m());
	Geom* bodyGeom = new SphereGeom(m_pBody,in_pWorld->getSpace(),m_radius.m());
	bodyGeom->setMass(m_mass);
}

void Ball::destroy()
{
	delete m_pBody;
}

num::Pose Ball::getPosition()
{
	double pos[3]; 
	m_pBody->getPosition(pos);
	return Pose(Meter(pos[0]),Meter(pos[1]),m_pBody->getOrientation2D());
}

num3D::Pose3D Ball::getPose3D()
{
	using namespace num3D;
	double pos[3];
	double q[4];
	m_pBody->getPosition(pos);
	m_pBody->getQuaternion(q);

	return Pose3D(Point3D(Meter(pos[0]), Meter(pos[1]), Meter(pos[2])),
			Quaternion(q[0],q[1],q[2],q[3]));
}
