/** @file
*      Wraper around ODE geometry with box shape
*         
* @author Jaroslav Sladek
*
* $Id$
*
*/

#include "SphereGeom.h"
#include "Body.h"

SphereGeom::SphereGeom(Body* in_pBody, Space* in_pSpace, double radius) 
	: Geom(in_pSpace)
{
	m_pBody = in_pBody;
	dMassSetSphere(&m_mass,1,radius);
	dMassAdjust(&m_mass,1);
	m_id = dCreateSphere(m_pSpace->id(),radius);
	finishGeom();
}

