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

class World;
 
class OdeObject
{
	public:
		virtual void create(World* in_world, num::Pose in_pose) = 0;
		virtual void destroy() = 0;
};

#endif
