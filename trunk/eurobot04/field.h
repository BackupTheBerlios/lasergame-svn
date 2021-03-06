#ifndef ESTER_FILED_H_INCLUDED
#define ESTER_FILED_H_INCLUDED 1

#include "number/pose.h"
#include "number/dist.h"
#include "number/time.h"
#include "util/thread.h"

#include <list>

class Balls : public std::list<num::Point> {};

class Field : public thread::Lock
{
	private:
		num::Pose m_robot[2];
		int m_score;
		num::Point m_palm[2];

		uint32_t m_ballSeed;
		uint32_t m_palmSeed;
		bool m_verbose;

		num::Dist  m_minPalmDist[2];
		num::Pose m_minPalmPose[2];
		//num::Time  m_minPalmTime; // ??? TODO
		//num::Dist  m_minEnemyDist;
		//num::Point m_minEnemyPose;
		//num::Time  m_minEnemyTime;
		num::Time  m_lastHitTime[2];
	
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
		void setRobot(const int in_id, const num::Pose& in_pose);
		const num::Pose& getRobot(const int in_id);

		void checkPalms(const num::Point & in_p);
		void checkEnemy(const num::Point & in_p);
		void updateStat();

		num::Dist palm1Dist(const num::Point & in_p) const { return m_palm[0].distanceTo(in_p); }
		num::Dist palm2Dist(const num::Point & in_p) const { return m_palm[1].distanceTo(in_p); }

		int tryEatBall(const num::Pose & in_pose, const num::Dist & BALL_EAT_DIST);
		void shootBall(const num::Pose & in_pose);
		
		Balls m_balls;
};

#endif // ESTER_FILED_H_INCLUDED

