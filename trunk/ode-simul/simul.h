#ifndef ODE_SIMUL_H_INCLUDED
#define ODE_SIMUL_H_INCLUDED

/** @file
*         Basic ODE simulation interface
*         
* @author Jaroslav Sladek
*
* $Id: angle.h 77 2004-09-08 20:47:04Z zwin $
*/


#include <vector>
#include "util/msg.h"
#include "number/pose.h"
#include "number/speed.h"
#include "ode-robot.h"

class dWorld;
class dSpace;
class dJointGroup;
class dPlane;

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
		dWorld* m_pWorld;
		dSpace* m_pSpace;
		dJointGroup* m_pContactGroup;
		dPlane* m_pGround;
		OdeRobot* m_pRobot;
		
};

#endif
