#ifndef GEOM_H_INCLUDED
#define GEOM_H_INCLUDED	
/** @file
*      Wraper around ODE Geom
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <ode/ode.h>
#include "ContactData.h"
#include "World.h"

class Body;

class Geom 
{
	public:
		Geom(World* in_pWorld);
		virtual ~Geom();
		void setMass(double in_mass);
		ContactData m_contactData;
		friend class Body;
	protected:
		void finishGeom();
		dGeomID m_id;
		dSpaceID m_spaceID;
		Body* m_pBody;
		dMass m_mass;

};

#endif
