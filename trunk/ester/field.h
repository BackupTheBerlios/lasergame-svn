#ifndef ESTER_FILED_H_INCLUDED
#define ESTER_FILED_H_INCLUDED 1

#include "number/pose.h"
#include "number/dist.h"
#include "number/time.h"

#include <list>

class Field
{
	private:
		num::Point m_enemy;
		int m_score;
		num::Point m_palm1;
		num::Point m_palm2;
		std::list<num::Point> m_balls;
		
		uint32_t m_ballSeed;
		uint32_t m_palmSeed;
		bool m_verbose;

		num::Dist  m_minPalmDist;
		num::Point m_minPalmPose;
		num::Time  m_minPalmTime;
		num::Dist  m_minEnemyDist;
		num::Point m_minEnemyPose;
		num::Time  m_minEnemyTime;
		num::Time  m_lastHitTime;
	
		enum { BALL_COUNT=2*4, RAND_PALM=1, RAND_BALL=2, ERROR=4, VERBOSE=8 };

		void printIntro();
		void printStat();
		
	public:
		Field(int in_flags = 0);
		~Field();
		void setPalm(int in_crossingX = 0, int in_crossingY = 0);
		void randPalm(uint32_t in_seed = 0);
		void setBall(uint32_t in_seed = 0);
		void setBall(int in_pos1, int in_pos2, int in_pos3);
		void setEnemy(const num::Point& m_place);

		void checkPalms(const num::Point & in_p);
		void checkEnemy(const num::Point & in_p);
};

#endif // ESTER_FILED_H_INCLUDED

