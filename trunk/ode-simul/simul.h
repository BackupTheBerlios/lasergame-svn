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

class OdeSimul : public msg::Runnable
{
	private:
		msg::Channel* m_pChannel;
	public:
		OdeSimul(msg::Channel* in_pChannel);
		~OdeSimul();
		void addRobot(OdeRobot* in_pRobot);

		virtual void main();
	private:
		World* m_pWorld;

		OdeRobot* m_pRobot;
		ContactData m_groundCD;
		
};

#endif
