/** @file
*      Wraper around ODE Hinge2Joint
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include "World.h"
#include "Body.h"
#include "Hinge2Joint.h"

Hinge2Joint::Hinge2Joint(World* in_pWorld)
{
	m_id = dJointCreateHinge2(in_pWorld->getID(), 0);
}

Hinge2Joint::~Hinge2Joint()
{
	dJointDestroy(m_id);
}

void Hinge2Joint::attach(Body* in_pBody1, Body* in_pBody2)
{
	dJointAttach(m_id, in_pBody1->id(), in_pBody2->id());
}

void Hinge2Joint::setAnchor(double x, double y, double z)
{
	dJointSetHinge2Anchor(m_id, x, y, z);
}

void Hinge2Joint::setAxis1(double x, double y, double z)
{
	dJointSetHinge2Axis1(m_id, x, y, z);
}

void Hinge2Joint::setAxis2(double x, double y, double z)
{
	dJointSetHinge2Axis2(m_id, x, y, z);
}

void Hinge2Joint::setParam(int in_param, double in_value)
{
	dJointSetHinge2Param(m_id, in_param, in_value);
}

double Hinge2Joint::getAngle1Rate()
{
	return dJointGetHinge2Angle1Rate(m_id);
}

double Hinge2Joint::getAngle2Rate()
{
	return dJointGetHinge2Angle2Rate(m_id);
}
