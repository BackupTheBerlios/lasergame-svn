#include <iostream>

#include <ode/ode.h>
#include <ode/odecpp.h>

#include "four-wheeler.h"
#include "number/speed.h"

using namespace num;
using namespace std;


namespace {
	num::Dist STARTZ() {return Milim(100);}
	num::Dist ROBOT_LENGTH() {return Milim(300);}
	num::Dist ROBOT_WIDTH() {return Milim(200);}
	num::Dist ROBOT_HEIGHT() {return Milim(100);}
	num::Dist RADIUS() {return Milim(50);}
	double WMASS() {return 1;}
	double CHASSIS_MASS() {return 10;}
	double FMAX() { return 1;}

};

FourWheeler::FourWheeler(msg::Channel* in_pChannel) : OdeRobot(in_pChannel), 
m_reqSpeed(m_pChannel,"speed-requested", this, &FourWheeler::reqSpeed),
m_currentSpeed(m_pChannel,"speed-current")
{
}

void FourWheeler::reqSpeed(num::Speed& in_speed) //{{{1
{
	cout << "Speed received... " << m_reqSpeed.value.m_forward.mm() << endl;
}



void FourWheeler::create(dWorld* in_world, dSpace* in_space, Pose in_pose)
{
	msg::Subs<num::Pose> pose(m_pChannel,"true-pose");
	msg::Subs<num::Speed, FourWheeler> reqSpeed(m_pChannel,"speed-requested", this, &FourWheeler::reqSpeed);
	msg::Subs<num::Speed> currentSpeed(m_pChannel, "speed-current");

	pose.value = in_pose;

  m_pChassis = new dBody(in_world->id());
	m_pChassis->setPosition(in_pose.x().m(),in_pose.y().m(),STARTZ().m());
	dMass mass;
	dMassSetBox(&mass, 1, ROBOT_LENGTH().m(),ROBOT_WIDTH().m(),ROBOT_HEIGHT().m());
	dMassAdjust(&mass, CHASSIS_MASS());
	m_pChassis->setMass(&mass);

	m_pChassisBox = new dBox(in_space->id(), ROBOT_LENGTH().m(),ROBOT_WIDTH().m(),ROBOT_HEIGHT().m());
	m_pChassisBox->setBody(m_pChassis->id());
	m_chassisContact.mu = 0.5;
	m_pChassisBox->setData(&m_chassisContact);

	m_wheels = new dBody[4];
	m_wheelGeoms = new dSphere[4];

	m_wheelContact.mu = 20;
	for (int i = 0; i < 4; i++)
	{
		m_wheels[i].create(in_world->id());
		dQuaternion q;
		dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
		m_wheels[i].setQuaternion(q);
		dMassSetSphere (&mass,1,RADIUS().m());
		dMassAdjust (&mass,WMASS());
		m_wheels[i].setMass(&mass);
		m_wheelGeoms[i].create(in_space->id(),RADIUS().m());
		m_wheelGeoms[i].setBody(m_wheels[i].id());
		m_wheelGeoms[i].setData(&m_wheelContact);
	}
	m_wheels[0].setPosition(in_pose.x().m()+0.4*ROBOT_LENGTH().m()-0.5*RADIUS().m(), 
	                      in_pose.y().m()+0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());
	m_wheels[1].setPosition(in_pose.x().m()+0.4*ROBOT_LENGTH().m()-0.5*RADIUS().m(), 
	                      in_pose.y().m()-0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());
	m_wheels[2].setPosition(in_pose.x().m()-0.4*ROBOT_LENGTH().m()-0.5*RADIUS().m(), 
	                      in_pose.y().m()+0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());
	m_wheels[3].setPosition(in_pose.x().m()-0.4*ROBOT_LENGTH().m()-0.5*RADIUS().m(), 
	                      in_pose.y().m()-0.5*ROBOT_WIDTH().m(),STARTZ().m()-0.5*ROBOT_HEIGHT().m());

  m_joints = new dHinge2Joint[4];
	for (int i=0; i<4;i++)
	{
		m_joints[i].create(in_world->id());
		m_joints[i].attach(m_pChassis->id(),m_wheels[i].id());
		const dReal *a = m_wheels[i].getPosition();
    m_joints[i].setAnchor (a[0],a[1],a[2]);
		m_joints[i].setAxis1(0,0,(i<2 ? 1 : -1));
		m_joints[i].setAxis2(0,1,0);
		m_joints[i].setParam(dParamSuspensionERP,0.8);
		m_joints[i].setParam(dParamSuspensionCFM,1e-5);
		m_joints[i].setParam(dParamVel2,0);
		m_joints[i].setParam(dParamFMax2,FMAX());
		m_joints[i].setParam(dParamLoStop,0);
		m_joints[i].setParam(dParamHiStop,0);

	}
		
}

void FourWheeler::destroy()
{
	delete [] m_wheelGeoms;
	delete [] m_wheels;
	delete [] m_joints;
	delete m_pChassisBox;
	delete m_pChassis;
}

void FourWheeler::update()
{
	const dReal* pos = m_pChassisBox->getPosition();
	std::cout << "Robot box position: " << pos[0] << "," << pos[1] << "," << pos[2] << std::endl;

	dReal speed = m_reqSpeed.value.m_forward.m()/RADIUS().m()*-1;
	for (int i=0;i<4;i++)
	{
		m_joints[i].setParam(dParamVel2,speed);
		m_joints[i].setParam(dParamFMax2,FMAX());
		// cout << m_joints[i].getAngle2Rate() << ",";
		dBodyEnable(m_joints[i].getBody(0));
		dBodyEnable(m_joints[i].getBody(1));
	}
	m_currentSpeed.value.m_forward = Meter(m_joints[0].getAngle2Rate()*RADIUS().m() * -1);
	cout << "Current speed is: " << m_currentSpeed.value.m_forward.mm() << endl;
	m_currentSpeed.publish();
}
