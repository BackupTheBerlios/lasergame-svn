/** @file
*      Wraper around ODE Geom
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include "Geom.h"
#include "Body.h"

Geom::Geom(World* in_pWorld) : m_id(0), m_spaceID(in_pWorld->getSpaceID()),
															 m_pBody(0)
{
}
	
Geom::~Geom()
{
	if (m_id)
		dGeomDestroy(m_id);
}

void Geom::finishGeom()
{
	m_pBody->setMass(&m_mass);
	dGeomSetData(m_id, this);
	m_pBody->attachGeom(this);
}

void Geom::setMass(double in_mass)
{
	dMassAdjust(&m_mass, in_mass);
	m_pBody->setMass(&m_mass);
}

