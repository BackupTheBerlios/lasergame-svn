/** @file
*        Wraper around ODE World
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <iostream>

#include "World.h"
#include "Geom.h"

namespace {
	class Plane : public Geom
	{
		public:
			Plane(World& in_world) : Geom(&in_world)
			{
				m_id = dCreatePlane(m_spaceID,0,0,1,0);
				dGeomSetData(m_id, this);
			}
	};
};

using namespace num;

void World::nearCallback(void *data, dGeomID o1, dGeomID o2)
{
	int i,n;

	World* pWorld = (World*) data;
	
	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	if (b1 && b2 && dAreConnected(b1, b2))
		return;

	if (dGeomIsSpace (o1) || dGeomIsSpace (o2)) {
		// colliding a space with something
		dSpaceCollide2 (o1,o2,data,&nearCallback);
	}
	else {
		// colliding two non-space geoms, so generate contact
		// points between o1 and o2
		//std::cout<<"Colliding"<<std::endl;
		const int N = 10;
		dContact contact[N];
		n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
		Geom *geom1, *geom2;
		geom1 = (Geom*) dGeomGetData(o1);
		geom2 = (Geom*) dGeomGetData(o2);
		//std::cout << "cd1->mu is: " << geom1->m_contactData.mu << ", cd2->mu is: " << geom2->m_contactData.mu << std::endl;
		double mu = (geom1->m_contactData.mu < geom2->m_contactData.mu ? 
				geom1->m_contactData.mu : geom2->m_contactData.mu);

		// add these contact points to the simulation
		if (n > 0) {
			for (i=0; i<n; i++) {
				contact[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1;
				contact[i].surface.mu = mu;
				contact[i].surface.soft_erp = 0.8;
				contact[i].surface.soft_cfm = 0.01;
				dJointID c = dJointCreateContact (pWorld->m_id,pWorld->m_contactID,contact+i);
				dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
			}
		}
	}
}

World::World()
{
	m_id = dWorldCreate();
	m_spaceID = dHashSpaceCreate(0);
	m_contactID = dJointGroupCreate(0);
	m_ground = new Plane(*this);
}

World::~World()
{
	delete m_ground;
	dJointGroupDestroy(m_contactID);
	dSpaceDestroy(m_spaceID);
	dWorldDestroy(m_id);
}

void World::setGravity(double x, double y, double z)
{
	dWorldSetGravity(m_id,x,y,z);
}

void World::step(Time in_interval)
{
	dSpaceCollide(m_spaceID,this,&nearCallback);
	dWorldStep(m_id, in_interval.ms()/1000.0);
	dJointGroupEmpty(m_contactID);
}
