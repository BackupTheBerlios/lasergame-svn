#ifndef BOX_GEOM_H_INCLUDED
#define BOX_GEOM_H_INCLUDED
/** @file
*      Wraper around ODE geometry with box shape
*         
* @author Jaroslav Sladek
*
* $Id$
*
*/

#include "Geom.h"

class BoxGeom : public Geom
{
	public:
		BoxGeom(Body* in_pBody, World* in_pWorld, double x, double y, double z);
		virtual int id() {return 1;} 
		virtual ~BoxGeom() {}
};


#endif
