#ifndef BODY_H_INCLUDED
#define BODY_H_INCLUDED	
/** @file
*      Wraper around ODE Body
*         
* @author Jaroslav Sladek
*
* $Id$
*/

#include <ode/ode.h>

class World;
class Geom;

class Body
{
	public:
		Body() : m_id(0) {}
		Body(World* p_world);
		~Body();
		void create(World* in_world);
		void setMass(const dMass* in_mass);
		void setPosition(double x, double y, double z);
		void getPosition(double out_vector[3]);
		void setQuaternion(dQuaternion in_q);
		void attachGeom(Geom* in_geom);
		dBodyID id() {return m_id;}
		friend class Geom;
	private:
		dBodyID m_id;
		Geom* m_geom;
		
};


#endif
