// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef ESTER_SIMUL_H_INCLUDED
#define ESTER_SIMUL_H_INCLUDED 1

#include "util/msg.h"
#include "number/rnd.h"
#include "number/pose.h"
#include "number/speed.h"
#include "floor-color.h"
#include "field.h"

/// Simulation of a robot Ester
class Simul : public msg::Runnable
{
	private:
		msg::Channel* m_p;
		Field& m_field;

		num::Angle::type AERR; ///< Expected error in the change of the orientation
		num::Dist::type  FERR; ///< Expected error in the distance traveled
		num::Angle::type DERR; ///< Expected error in distance when changing orientation (meter/rad)
	
		int m_side;
		
	public:
		Simul(msg::Channel* in_pChannel, Field* in_field, int in_side);
		//Simul(const Robot & a, const Robot & b, const Field & in_field);
		~Simul();
		virtual void main();

	private:
		void reqSpeed();
		void reqShoot();

#if 0
	void upBalls();
	void upCamera();
	void upEnemy();
	void checkPalms();
	void checkEnemy();
	
	public:
		void setError(const num::Number& in_ae=num::Number(3,10), 
			const num::Number& in_fe=num::Number(4,10), const num::Number& in_de=num::Number(2,10));
#endif
};


#endif

