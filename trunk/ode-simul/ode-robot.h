#ifndef ODE_ROBOT_H_INCLUDED
#define ODE_ROBOT_H_INCLUDED
/** @file
*         Ancestor of all robots
*         
* @author Jaroslav Sladek
*
* $Id$
*/


#include "util/msg.h"
#include "number/pose.h"
#include "number/time.h"

class World;
 
class OdeRobot
{
	protected:
	  msg::Channel* m_pChannel;
	public:
	  OdeRobot(msg::Channel* in_pChannel) : m_pChannel(in_pChannel) {}
	  virtual void update(const num::Time& in_timeChange) = 0;
		virtual void create(World* in_world, num::Pose in_pose) = 0;
		virtual void destroy() = 0;
		virtual ~OdeRobot() {}
};

#endif