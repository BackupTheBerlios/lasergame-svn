/** @file
*      Wraper around ODE geometry with box shape
*         
* @author Jaroslav Sladek
*
* $Id$
*
*/

#include "BoxGeom.h"
#include "Body.h"

BoxGeom::BoxGeom(Body* in_pBody, Space* in_pSpace, double x, double y, double z) : Geom(in_pSpace)
{
	m_pBody = in_pBody;
	dMassSetBox(&m_mass,1,x,y,z);
	m_id = dCreateBox(m_pSpace->id(),x,y,z);
	finishGeom();
}
