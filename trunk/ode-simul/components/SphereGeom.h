#ifndef SPHERE_GEOM_H_INCLUDED
#define SPHERE_GEOM_H_INCLUDED
/** @file
*      Wraper around ODE geometry with box shape
*         
* @author Jaroslav Sladek
*
* $Id$
*
*/

#include "Geom.h"

class SphereGeom : public Geom
{
	public:
		SphereGeom(Body* in_pBody, Space* in_pSpace, double radius);
		virtual ~SphereGeom() {}
};


#endif
