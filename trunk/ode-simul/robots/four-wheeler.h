#ifndef ODE_FOUR_WHEELER_H_INCLUDED
#define ODE_FOUR_WHEELER_H_INCLUDED

#include "number/speed.h"
#include "../ode-robot.h"
#include "../ContactData.h"

class dBody;
class dBox;
class dHinge2Joint;
class dSphere;

class FourWheeler : public OdeRobot
{
  public:
	  FourWheeler(msg::Channel* in_pChannel);
	  virtual void update();
		virtual void create(dWorld* in_world, dSpace* in_space, num::Pose in_pose);
		virtual void destroy();
	private:
		void reqSpeed(num::Speed& in_speed);
		msg::Subs<num::Speed, FourWheeler> m_reqSpeed;
		dBody* m_pChassis;
		dBox* m_pChassisBox;
		dHinge2Joint* m_joints;
		dBody* m_wheels;
		dSphere* m_wheelGeoms;
		ContactData m_chassisContact;
		ContactData m_wheelContact;

};

#endif
