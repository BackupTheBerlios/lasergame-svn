#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED
/** @file
*      Wraper around ODE world
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <ode/ode.h>
#include "number/time.h"

class Geom;

class World
{
	public:
	  World();
		~World();
		void setGravity(double x, double y, double z);
		void step(num::Time in_interval);
		dSpaceID getSpaceID() {return m_spaceID;}
		dWorldID getID() { return m_id; }
	private:
	  dWorldID m_id;
		dSpaceID m_spaceID;
		dJointGroupID m_contactID;
		Geom* m_ground;
		static void nearCallback(void *data, dGeomID o1, dGeomID o2);
};


#endif
