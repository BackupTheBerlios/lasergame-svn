#ifndef ESTER_FILED_H_INCLUDED
#define ESTER_FILED_H_INCLUDED 1

#include "number/pose.h"
#include <list>

class Field
{
	private:
		num::Pose m_enemy;
		int m_score;
		num::Point m_palms[2];
		std::list<num::Point> m_balls;
		
		uint32_t m_ballSeed;
		uint32_t m_palmSeed;

		num::Dist m_minPalmDist;
		num::Pose m_minPalmPose;
		num::Time m_minPalmTime;
		num::Dist m_minEnemyDist;
		num::Pose m_minEnemyPose;
		num::Time m_minEnemyTime;
		num::Time m_lastHitTime;
	
		enum { BALL_COUNT=2*4, RAND_PALM=1, RAND_BALL=2, ERROR=4, VERBOSE=8 };

	public:
		Field(int in_flags);
		~Field();
};

#endif // ESTER_FILED_H_INCLUDED

