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

class Space;
class Geom;

class World
{
	public:
	  World();
		~World();
		void setGravity(double x, double y, double z);
		void step(num::Time in_interval);
		Space* getSpace() {return m_space;}
		dWorldID getID() { return m_id; }
	private:
	  dWorldID m_id;
		Space* m_space;
		dJointGroupID m_contactID;
		Geom* m_ground;
		static void nearCallback(void *data, dGeomID o1, dGeomID o2);
};


#endif
