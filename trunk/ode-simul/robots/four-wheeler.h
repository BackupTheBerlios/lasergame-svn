#ifndef ODE_FOUR_WHEELER_H_INCLUDED
#define ODE_FOUR_WHEELER_H_INCLUDED
/** @file
*        Simple four wheel robot
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include "number/speed.h"
#include "../ode-robot.h"
#include "../ContactData.h"
#include "../components/Body.h"
#include "../components/BoxGeom.h"
#include "../components/Hinge2Joint.h"
#include "../number3D/pose3D.h"

class dBody;
class dBox;
class dHinge2Joint;
class dSphere;
class Space;

class FourWheeler : public OdeRobot
{
  public:
	  FourWheeler(msg::Channel* in_pChannel);
	  virtual void update(const num::Time& in_timeChange);
		virtual void create(World* in_world, num3D::Pose3D in_pose);
		virtual void destroy();
		virtual num::Pose getPosition();
	private:
		void reqSpeed(num::Speed& in_speed);
		msg::Subs<num::Speed, FourWheeler> m_reqSpeed;
	  msg::Subs<num::Speed> m_currentSpeed;
	  msg::Subs<num::Pose> m_poseChange;
		Body* m_pChassis;
		BoxGeom* m_pChassisBox;
		Hinge2Joint* m_joints[4];
		Body* m_wheels;
		Space* m_pSpace;

};

#endif
