#ifndef ESTER_SIMUL_H_INCLUDED
#define ESTER_SIMUL_H_INCLUDED 1

#include "util/msg.h"
#include "number/rnd.h"
#include "number/pose.h"
#include "number/speed.h"
//#include "floorcolor.h"

class Field;
class ModelBase;

/// Simulation of a robot for Eurobot 2004
class Simul : public msg::Runnable
{
	private:
		Field& m_field;
		ModelBase& m_model;

		/// @name Common interface
		/// @{
		msg::Subs<int>       m_watchdog;
		msg::Subs<uint32_t>  m_seed;
		msg::Subs<num::Time> m_timeChange;
		msg::Subs<num::Pose> m_poseChange;
		msg::Subs<num::Pose> m_pose;
		msg::Subs<num::Speed> m_currentSpeed;
		
		msg::Subs<num::Speed, Simul> m_reqSpeed;
		///@}
		
		/// @name Eurobot specific interface
		/// @{
		msg::Subs<bool> m_start;
		msg::Subs<num::Point> m_ballPos; ///< Position of a primary ball in a local frame
		msg::Subs<num::Point> m_enemy;   ///< Position of enemy in a local frame
		//Subs<FloorColor> m_floorColor;
		msg::Subs<int> m_numBallsIn;
		msg::Subs<double> m_gp2top;

		msg::Subs<num::Dist, Simul> m_reqShoot;
		/// @}

		num::Angle::type AERR; ///< Expected error in the change of the orientation
		num::Dist::type  FERR; ///< Expected error in the distance traveled
		num::Angle::type DERR; ///< Expected error in distance when changing orientation (meter/rad)
	
		int m_side;
		
		num::Rnd m_rnd;
		//int m_camTick;
		
	public:
		Simul(msg::Channel* in_pChannel, Field& in_field, ModelBase& in_model, int in_side = 0);
		~Simul();
		virtual void main();

	private:
		void reqSpeed();
		void reqShoot();
		void upSpeed();
		void upPose();

#if 0
	void upMoveState();
	void upFloorColor();
	void upGP2();
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

