/** @file
*      Wraper around ODE Body
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <ode/ode.h>

#include "World.h"
#include "Body.h"
#include "Geom.h"

Body::Body(World* in_pWorld)
{
	m_id = dBodyCreate(in_pWorld->getID());
	m_geom = NULL;
}

Body::~Body()
{
	if (m_geom)
		delete m_geom;
	if (m_id)
		dBodyDestroy(m_id);
}

void Body::create(World* in_pWorld)
{
	m_id = dBodyCreate(in_pWorld->getID());
}

void Body::setMass(const dMass* in_mass)
{
	dBodySetMass(m_id, in_mass);
}

void Body::setPosition(double x, double y, double z)
{
	dBodySetPosition(m_id, x, y, z);
}

void Body::getPosition(double out_vector[3])
{
	const dReal* a = dBodyGetPosition(m_id);
	out_vector[0] = a[0];
	out_vector[1] = a[1];
	out_vector[2] = a[2];
}

void Body::setQuaternion(dQuaternion in_q)
{
	dBodySetQuaternion(m_id, in_q);
}

void Body::attachGeom(Geom* in_geom)
{
	m_geom = in_geom;
	dGeomSetBody(m_geom->m_id, m_id);
}

