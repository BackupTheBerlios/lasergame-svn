#ifndef ODE_ROBOT_H_INCLUDED
#define ODE_ROBOT_H_INCLUDED
/** @file
*         Ancestor of all robots
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include "ode-object.h"
 
class OdeRobot : public OdeObject
{
	protected:
	  msg::Channel* m_pChannel;
	public:
	  OdeRobot(msg::Channel* in_pChannel) : m_pChannel(in_pChannel) {}
	  virtual void update(const num::Time& in_timeChange) = 0;
		virtual ~OdeRobot() {}
};

#endif
