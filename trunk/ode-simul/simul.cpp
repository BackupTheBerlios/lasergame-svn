/** @file
*         Basic ODE simulation interface
*         
* @author Jaroslav Sladek
*
* $Id: angle.h 77 2004-09-08 20:47:04Z zwin $
*/

#include <iostream>
#include <ode/ode.h>
#include <ode/odecpp.h>
#include <ode/odecpp_collision.h>
#include "simul.h"

using namespace std;
using namespace num;

namespace {
	num::Pose INITIAL_POSITION() { return Pose(Milim(0), Milim(0), Deg(0)); }
	struct CollideData
	{
		CollideData(dWorld* in_pWorld, dJointGroup* in_pContactGroup) : pWorld(in_pWorld), pContactGroup(in_pContactGroup) {}
		dWorld* pWorld;
		dJointGroup* pContactGroup;
	};

  void nearCallback (void *data, dGeomID o1, dGeomID o2)
	{
		int i,n;

		CollideData* pCD = (CollideData*) data;
		
		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);
		if (b1 && b2 && dAreConnected(b1, b2))
			return;

		//std::cout<<"Colliding"<<std::endl;
		const int N = 4;
		dContact contact[N];
		n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
		if (n > 0) {
			for (i=0; i<n; i++) {
				contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
				if (dGeomGetClass(o1) == dSphereClass || dGeomGetClass(o2) == dSphereClass)
					contact[i].surface.mu = 20;
				else
					contact[i].surface.mu = 0.5;
				contact[i].surface.slip1 = 0.0;
				contact[i].surface.slip2 = 0.0;
				contact[i].surface.soft_erp = 0.8;
				contact[i].surface.soft_cfm = 0.01;
				dJointID c = dJointCreateContact (pCD->pWorld->id(),pCD->pContactGroup->id(),contact+i);
				dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
			}
		}
	}

};

OdeSimul::OdeSimul(msg::Channel* in_pChannel) : m_pChannel(in_pChannel) 
{
	m_pWorld = new dWorld();
	m_pWorld->setGravity(0,0,-9.81);
	m_pSpace = new dHashSpace(0);
	m_pContactGroup = new dJointGroup;

	//create ground
	m_pGround = new dPlane(m_pSpace->id(),0,0,1,0);

}

OdeSimul::~OdeSimul()
{
	delete m_pGround;
	delete m_pContactGroup;
	delete m_pSpace;
	delete m_pWorld;
}



void OdeSimul::addRobot(OdeRobot* in_pRobot)
{
	m_pRobot = in_pRobot;
	m_pRobot->create(m_pWorld, m_pSpace, INITIAL_POSITION());
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
			m_pRobot->update();
	  CollideData cd(m_pWorld,m_pContactGroup);
		m_pSpace->collide(&cd,&nearCallback);
		m_pWorld->step(timeChange.value.ms()/1000.0);
		m_pContactGroup->empty();
	}
}
