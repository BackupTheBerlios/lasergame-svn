/** @file
*         Basic ODE simulation interface
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <iostream>
#include <ode/ode.h>
#include "simul.h"

using namespace std;
using namespace num;
using namespace num3D;

namespace {
	num3D::Pose3D INITIAL_POSITION() { return Pose3D(Milim(0), Milim(0)); }
};

OdeSimul::OdeSimul(msg::Channel* in_pChannel) : m_pChannel(in_pChannel) , m_pRobot(0)
{
	m_pWorld = new World();
	m_pWorld->setGravity(0,0,-9.81);
}

OdeSimul::~OdeSimul()
{
	delete m_pWorld;
}

void OdeSimul::addRobot(OdeRobot* in_pRobot)
{
	addRobot(in_pRobot, INITIAL_POSITION());
}

void OdeSimul::addRobot(OdeRobot* in_pRobot, const num3D::Pose3D & in_pose)
{
	m_pRobot = in_pRobot;
	m_pRobot->create(m_pWorld, in_pose);
}

void OdeSimul::addObject(OdeObject* in_pObject, const num3D::Pose3D & in_pose)
{
	in_pObject->create(m_pWorld, in_pose);
	m_objects.push_back(in_pObject);
}

void OdeSimul::main()
{
	msg::Subs<int> watchdog(m_pChannel,"watchdog");
	msg::Subs<num::Time> timeChange(m_pChannel, "time-change");
	//msg::Subs<num::Pose> poseChange(m_pChannel, "pose-chagne");
	timeChange.value = MSec(5);

	while (true)
	{
		timeChange.publish();
		waitFor(watchdog);
		if (m_pRobot)
			m_pRobot->update(timeChange.value);
		m_pWorld->step(timeChange.value);
	}
}
