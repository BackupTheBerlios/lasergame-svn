#ifndef ODE_BALL_H_INCLUDED
#define ODE_BALL_H_INCLUDED
/** @file
*        Ball
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include "../ode-object.h"
#include "../components/Body.h"
#include "../number3D/pose3D.h"

class Ball : public OdeObject
{
	public:
		Ball(num::Dist in_radius, double in_mass = 1) : 
			m_pBody(0), m_radius(in_radius), m_mass(in_mass) {}
		virtual void create(World* in_world, num3D::Pose3D in_pose);
		virtual void destroy();
		virtual num::Pose getPosition();
		virtual num3D::Pose3D getPose3D();
	private:
		Body* m_pBody;
		num::Dist m_radius;
		double m_mass;
};


#endif
