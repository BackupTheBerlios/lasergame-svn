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
#include "Space.h"

class Body;

class Geom 
{
	public:
		Geom(Space* in_pSpace);
		virtual ~Geom();
		void setMass(double in_mass);
		ContactData m_contactData;
		friend class Body;
	protected:
		void finishGeom();
		dGeomID m_id;
		Space* m_pSpace;
		Body* m_pBody;
		dMass m_mass;

};

#endif
