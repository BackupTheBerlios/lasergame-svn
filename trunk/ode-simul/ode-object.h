#ifndef ODE_OBJECT_H_INCLUDED
#define ODE_OBJECT_H_INCLUDED
/** @file
*         Ancestor of all robots
*         
* @author Jaroslav Sladek
*
* $Id$
*/


#include "util/msg.h"
#include "number/pose.h"
#include "number/time.h"
#include "number3D/pose3D.h"

class World;
 
class OdeObject
{
	public:
		virtual void create(World* in_world, num3D::Pose3D in_pose) = 0;
		virtual void destroy() = 0;
		virtual num::Pose getPosition() = 0;
};

#endif
