#ifndef SPACE_H_INCLUDED
#define SPACE_H_INCLUDED
/** @file
*      Wraper around ODE SimpleSpace
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <ode/ode.h>

class Space
{
	public:
		dSpaceID id() { return m_id; }
	protected:
	  dSpaceID m_id;
};

class SimpleSpace : public Space
{
	public:
		SimpleSpace() 
		{ m_id = dSimpleSpaceCreate(0); }
		SimpleSpace(Space* m_parent) 
		{ m_id = dSimpleSpaceCreate(m_parent->id()); }
		~SimpleSpace()
		{ dSpaceDestroy(m_id); }
};

class HashSpace : public Space
{
	public:
		HashSpace() 
		{ m_id = dHashSpaceCreate(0); }
		HashSpace(Space* m_parent) 
		{ m_id = dHashSpaceCreate(m_parent->id()); }
		~HashSpace()
		{ dSpaceDestroy(m_id); }
};

	


#endif
