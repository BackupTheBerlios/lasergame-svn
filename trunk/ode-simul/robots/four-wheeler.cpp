/** @file
*        Simple four wheel robot
*         
* @author Jaroslav Sladek
*
* $Id$
*/
#include <iostream>
#include <cmath>

#include <ode/ode.h>

#include "four-wheeler.h"
#include "number/speed.h"

#include "../components/World.h"
#include "../components/Body.h"
#include "../components/BoxGeom.h"
#include "../components/SphereGeom.h"
#include "../components/Hinge2Joint.h"

using namespace num;
using namespace std;


namespace {
	num::Dist STARTZ() {return Milim(100);}
	num::Dist ROBOT_LENGTH() {return Milim(140);}
	num::Dist ROBOT_WIDTH() {return Milim(200);}
	num::Dist ROBOT_HEIGHT() {return Milim(100);}
	num::Dist RADIUS() {return Milim(50);}
	double WMASS() {return 1;}
	double CHASSIS_MASS() {return 10;}
	double FMAX() { return 1;}

};

FourWheeler::FourWheeler(msg::Channel* in_pChannel) : OdeRobot(in_pChannel), 
m_reqSpeed(m_pChannel,"speed-requested", this, &FourWheeler::reqSpeed),
m_currentSpeed(m_pChannel,"speed-current"), m_poseChange(m_pChannel,"pose-change")
{
}

void FourWheeler::reqSpeed(num::Speed& in_speed) //{{{1
{
	cout << "Speed received... " << m_reqSpeed.value.m_forward.mm() << "," << m_reqSpeed.value.m_angular.deg() << endl;
}



void FourWheeler::create(World* in_world, Pose in_pose)
{
	msg::Subs<num::Pose> pose(m_pChannel,"true-pose");
	msg::Subs<num::Speed, FourWheeler> reqSpeed(m_pChannel,"speed-requested", this, &FourWheeler::reqSpeed);
	msg::Subs<num::Speed> currentSpeed(m_pChannel, "speed-current");

	pose.value = in_pose;

  m_pChassis = new Body(in_world);
	m_pChassis->setPosition(in_pose.x().m(),in_pose.y().m(),STARTZ().m());
	m_pChassisBox = new BoxGeom(m_pChassis, in_world->getSpace(), ROBOT_LENGTH().m(),ROBOT_WIDTH().m(),ROBOT_HEIGHT().m());
	m_pChassisBox->setMass(CHASSIS_MASS());

	m_wheels = new Body[4];

	for (int i = 0; i < 4; i++)
	{
		m_wheels[i].create(in_world);
		dQuaternion q;
		dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
		m_wheels[i].setQuaternion(q);
		Geom* geom = new SphereGeom(&m_wheels[i], in_world->getSpace(), RADIUS().m());
		geom->setMass(WMASS());
    // Magic number (see Pisvejc): make it too big, and turning won't be possible, make it too small, and 
		// robot will just stay on place, regardless of how fast wheels are turning
		geom->m_contactData.mu = 0.75; 
	}
	m_wheels[0].setPosition(in_pose.x().m()+0.4*ROBOT_LENGTH().m(), 
	                      in_pose.y().m()+0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());
	m_wheels[1].setPosition(in_pose.x().m()+0.4*ROBOT_LENGTH().m(), 
	                      in_pose.y().m()-0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());
	m_wheels[2].setPosition(in_pose.x().m()-0.4*ROBOT_LENGTH().m(), 
	                      in_pose.y().m()+0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());
	m_wheels[3].setPosition(in_pose.x().m()-0.4*ROBOT_LENGTH().m(), 
	                      in_pose.y().m()-0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());

	for (int i=0; i<4;i++)
	{
		m_joints[i] = new Hinge2Joint(in_world);
		m_joints[i]->attach(m_pChassis,&m_wheels[i]);
		double a[3];
		m_wheels[i].getPosition(a);
    m_joints[i]->setAnchor (a[0],a[1],a[2]);
		m_joints[i]->setAxis1(0,0,(i<2 ? 1 : -1));
		m_joints[i]->setAxis2(0,1,0);
		m_joints[i]->setParam(dParamSuspensionERP,0.8);
		m_joints[i]->setParam(dParamSuspensionCFM,1e-5);
		m_joints[i]->setParam(dParamVel2,0);
		m_joints[i]->setParam(dParamFMax2,FMAX());
		m_joints[i]->setParam(dParamLoStop,0);
		m_joints[i]->setParam(dParamHiStop,0);
	}
		
}

void FourWheeler::destroy()
{
	delete [] m_wheels;
	for (int i =0; i<4; i++)
	  delete m_joints[i];
	delete m_pChassisBox;
	delete m_pChassis;
}

Pose calcPoseChange(const Speed & in_speed, const Time & in_dt) //{{{1
{
	return Pose(in_speed.m_forward * in_dt, Dist(), in_speed.m_angular * in_dt);
}


void FourWheeler::update(const Time& in_timeChange)
{
	double pos[3]; 
	m_pChassis->getPosition(pos);
	std::cout << "Robot position and orientation: " << pos[0] << "," << pos[1] << "," << pos[2] << " -> "
		<< m_pChassis->getOrientation2D().deg() << std::endl;
	double speedR = ((m_reqSpeed.value.m_angular.rad() * ROBOT_WIDTH().m())/2 +  m_reqSpeed.value.m_forward.m());
	double speedL = (2 * m_reqSpeed.value.m_forward.m() - speedR);
	std::cout << "Setting speeds: R=" << speedR << " L=" << speedL << std::endl;
	for (int i=0;i<4;i++)
	{ 
		if (i % 2 == 0)
		  m_joints[i]->setParam(dParamVel2,speedL/RADIUS().m()*-1);
		else
		  m_joints[i]->setParam(dParamVel2,speedR/RADIUS().m()*-1);
		m_joints[i]->setParam(dParamFMax2,FMAX());

		cout << m_joints[i]->getAngle2Rate() * RADIUS().m() * -1 << ",";
#if 0
		dBodyEnable(m_joints[i].getBody(0));
		dBodyEnable(m_joints[i].getBody(1));
#endif
	}
	cout << endl;
	// Calculate odometric speed
	double dL, dR;

	// Use average distance travelled by both wheels on each side
	dL = (m_joints[0]->getAngle2Rate()*RADIUS().m() * -1 + m_joints[2]->getAngle2Rate()*RADIUS().m() * -1)/2;
	dR = (m_joints[1]->getAngle2Rate()*RADIUS().m() * -1 + m_joints[3]->getAngle2Rate()*RADIUS().m() * -1)/2;
	
	// Approximation of differential steering
	m_currentSpeed.value.m_forward = Meter((dL+dR)/2);
	m_currentSpeed.value.m_angular = Rad((dR-dL)/ROBOT_WIDTH().m());
	cout << "Current speed is: forward = " << m_currentSpeed.value.m_forward.mm() << ", angular = " <<
		   m_currentSpeed.value.m_angular.deg() << endl;
	m_currentSpeed.publish();

	m_poseChange.value = calcPoseChange(m_currentSpeed.value,in_timeChange);
	cout << "Current pose change is: " << m_poseChange.value <<endl;
	m_poseChange.publish();
}
