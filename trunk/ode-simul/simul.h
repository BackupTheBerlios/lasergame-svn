#ifndef ODE_SIMUL_H_INCLUDED
#define ODE_SIMUL_H_INCLUDED

/** @file
*         Basic ODE simulation interface
*         
* @author Jaroslav Sladek
*
* $Id$
*/


#include <vector>
#include "util/msg.h"
#include "number/pose.h"
#include "number/speed.h"
#include "ode-robot.h"
#include "ContactData.h"

#include "components/World.h"
#include "number3D/pose3D.h"

class OdeSimul : public msg::Runnable
{
	private:
		msg::Channel* m_pChannel;
	public:
		OdeSimul(msg::Channel* in_pChannel);
		~OdeSimul();
		void addRobot(OdeRobot* in_pRobot);
		void addRobot(OdeRobot* in_pRobot, const num3D::Pose3D & in_pose);
		void addObject(OdeObject* in_pObject, const num3D::Pose3D & in_pose);

		virtual void main();
	private:
		World* m_pWorld;

		OdeRobot* m_pRobot;
		std::vector<OdeObject*> m_objects;
		ContactData m_groundCD;
		
};

#endif
