#ifndef HINGE2JOINT_H_INCLUDED
#define HINGE2JOINT_H_INCLUDED
/** @file
*      Wraper around ODE Hinge2Joint
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <ode/ode.h>

class Body;
class World;

class Hinge2Joint
{
	public:
		Hinge2Joint(World* in_pWorld);
		~Hinge2Joint();
		void attach(Body* in_pBody1, Body* in_pBody2);
		void setAnchor(double x, double y, double z);
		void setAxis1(double x, double y, double z);
		void setAxis2(double x, double y, double z);
		void setParam(int in_parameter, double value);
		double getAngle1Rate();
		double getAngle2Rate();
	private:
		dJointID m_id;
};

#endif
