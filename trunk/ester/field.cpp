#include "field.h"
#include "measures.h"
#include "util/assert.h"

#include <algorithm>
#include <ctime>
#include <iostream>

using namespace measures;
using namespace num;
using namespace std;

namespace {

	const int BALL_POSITIONS = 15;
	const int PALM_POSITIONS = 10;
	class RandGen : public Rnd //{{{1
	{
		public:
			int operator()(int in_max)
			{
				//return (int) ((uniformD()+1)/2 * in_max);
				return rand() % in_max; 
      }
	};

	struct Coords //{{{1
	{
		Coords() {}
		Coords(int in_x, int in_y) : x(in_x), y(in_y) {}
		int x, y;
	};
	//}}}
}

Field::Field(int in_flags) : m_score(0), m_verbose(false) //{{{1
{
	if (in_flags & RAND_PALM) randPalm(0); else setPalm(2,2);
	if (in_flags & RAND_BALL)	setBall(0); else setBall(2,6,11);
	if (in_flags & VERBOSE) m_verbose = true;
}

Field::~Field() {} //{{{1

void Field::setPalm(int in_crossingX, int in_crossingY) //{{{1
{
	ASSERT( in_crossingX >= 2 && in_crossingX <=6 );
	ASSERT( in_crossingY >= 2 && in_crossingY <=5 );
	ASSERT( !(in_crossingX == 2 && in_crossingY == 5) && !(in_crossingX ==6 && in_crossingY == 2) );
	
	// Normalize palm position
	if (in_crossingX > 4 || (in_crossingX == 4 && in_crossingY > 3 ))
	{
		in_crossingX = 8 - in_crossingX;
		in_crossingY = 7 - in_crossingY;
	}
	m_palm1 = Point(measures::TILE() * in_crossingX, measures::TILE() * in_crossingY );
	m_palm2 = Point(measures::TILE() * (8 - in_crossingX), measures::TILE() * (7 - in_crossingY));
}

void Field::randPalm(uint32_t in_seed) //{{{1
{
	m_palmSeed = in_seed;
	if (m_palmSeed == 0)
		m_palmSeed = time(0);

	RandGen rnd;
	rnd.setSeed(m_palmSeed);

	Coords cards[PALM_POSITIONS];
	// Prepare cards with positions
	int curCard = 0;
	for (int i=0; i < PALM_POSITIONS; i++)
	{
		Coords position((i / 4) + 2, (i % 4) + 2);
		if (position.x == 2 && position.y == 5)
			continue; // skip fixed ball positions
		cards[curCard++] = position;
	}
	ASSERT( curCard == PALM_POSITIONS-1 );

	random_shuffle(cards, cards+curCard, rnd);
	
	setPalm(cards[0].x, cards[1].y);
}

void Field::setBall(uint32_t in_seed) //{{{1
{
	m_ballSeed = in_seed;
	RandGen rnd;
	if (m_ballSeed == 0)
		m_ballSeed = time(NULL);
	rnd.setSeed(m_ballSeed);
	const int placeCount = BALL_POSITIONS - 2; // without 1 preset and 1 palm
	int numbers[BALL_POSITIONS];

	// Prepare cards with positions
	for (int i=0; i < placeCount; i++)
	{
		numbers[i] = i;
	}
	random_shuffle(numbers, numbers+placeCount, rnd);
	sort(numbers, numbers+3); // Sort first three (normalize)
	setBall(numbers[0], numbers[1], numbers[2]);
}

void Field::setBall(int in_pos1, int in_pos2, int in_pos3) //{{{1
{
	using namespace measures;
	Coords cards[BALL_POSITIONS];
	int curCard = 0;

	ASSERT( in_pos1 < in_pos2 && in_pos2 < in_pos3 );
	ASSERT( in_pos3 < BALL_POSITIONS - 2 );

	// Prepare cards with positions
	for (int i=0; i < BALL_POSITIONS; i++)
	{
		Coords position((i / 6) + 2, (i % 6) + 1);
		if (position.x == 2 && position.y == 5)
			continue; // skip fixed ball positions
		if ((TILE()*position.x).eq(m_palm1.x()) && (TILE()*position.y).eq(m_palm1.y()))
			continue; // skip position where are the palms
		cards[curCard++] = position;
	}
	ASSERT( curCard == BALL_POSITIONS - 2 );

	m_balls.clear();

	m_balls.push_back(Point(measures::TILE() * 2, measures::TILE() * 5 ));;
	m_balls.push_back(Point(measures::TILE() * (8 - 2) , 
													measures::TILE() * (7 - 5)));

	m_balls.push_back(Point(measures::TILE() * cards[in_pos1].x, measures::TILE() * cards[in_pos1].y ));
	m_balls.push_back(Point(measures::TILE() * (8 - cards[in_pos1].x) , 
													measures::TILE() * (7-cards[in_pos1].y )));

	m_balls.push_back(Point(measures::TILE() * cards[in_pos2].x, measures::TILE() * cards[in_pos2].y ));
	m_balls.push_back(Point(measures::TILE() * (8 - cards[in_pos2].x) , 
													measures::TILE() * (7-cards[in_pos2].y )));

	m_balls.push_back(Point(measures::TILE() * cards[in_pos3].x, measures::TILE() * cards[in_pos3].y ));
	m_balls.push_back(Point(measures::TILE() * (8 - cards[in_pos3].x) , 
													measures::TILE() * (7-cards[in_pos3].y )));
}

void Field::printIntro() //{{{1
{
#if 0
	cout << "*** Using palm seed: " << m_palmSeed << endl;
	cout << "*** Palms: " << "[" << m_palmPosition1.n() << "], [" << m_palmPosition2.n() << "]" << endl;
	cout << "*** Using ball seed: " << m_ballSeed << endl;
  list<num::Place>::const_iterator ball_iter;
	cout << "*** Balls: ";
	for (ball_iter = m_balls.begin(); ball_iter != m_balls.end(); ++ball_iter)
	  cout << "[" << ball_iter->n() << "] ";
	cout << endl;
#endif
}

void Field::printStat() //{{{1
{
#if 0
	cout << "*** Final score was " << m_score << ", balls: on the field " << m_balls.size() << ", in robot " 
		<< m_numBallsIn << endl;
	cout << "*** Last hit scored at " << m_lastHitTime.ms() / 1000.0 << " s" << endl;
	cout << "*** Min palm dist was " << m_minPalmDist.mm() << " mm at [" << m_minPalmPose.n() << 
		"] at " << m_minPalmTime.ms()/1000.0 << " s" << endl;
	if (m_checkEnemy)
	  cout << "*** Min enemy dist was " << m_minEnemyDist.mm() << " mm at [" << m_minEnemyPose.n() << 
		  "] at " << m_minEnemyTime.ms()/1000.0 << " s" << endl;
#endif
}

void Field::checkPalms(const Point & in_p) //{{{1
{
	if (in_p.distanceTo(m_palm1) < m_minPalmDist)
	{
		m_minPalmDist = in_p.distanceTo(m_palm1);
		m_minPalmPose = in_p;
		//m_minPalmTime = m_timeChange.m_time;
	}
	if (in_p.distanceTo(m_palm2) < m_minPalmDist)
	{
		m_minPalmDist = in_p.distanceTo(m_palm2);
		m_minPalmPose = in_p;
		//m_minPalmTime = m_timeChange.m_time;
	}
}

void Field::checkEnemy(const Point & in_p) //{{{1
{
	if (in_p.distanceTo(m_enemy) < m_minEnemyDist)
  {
	  m_minEnemyDist = in_p.distanceTo(m_enemy);
	  m_minEnemyPose = in_p;
	  //m_minEnemyTime = m_timeChange.m_time;
  }
}

int Field::tryEatBall(const Pose & in_pose, const Dist & BALL_EAT_DIST) //{{{1
{
	ASSERT( m_balls.size() <= 8 );
	//cout << "*** Number of balls: " << m_balls.size() << endl;
 	list<Point>::iterator iter;

	for (iter = m_balls.begin(); iter != m_balls.end(); ++iter)
	{
		if (iter->distanceTo(in_pose.point()) <= BALL_EAT_DIST)
		{
			if (m_verbose) cout << "*** Just eaten ball at " << "[" << *iter << "]" << endl;
			m_balls.erase(iter);
			return 1;
		}
	}
	return 0;
}

void Field::shootBall(const Pose & in_pose) //{{{1
{
	if (m_verbose)
		cout << "*** Shooting ball from " << "[" << in_pose << "]" << endl;

	if ((in_pose.heading() <= Deg(-90)) || (in_pose.heading() >= Deg(90)))
	{
	  if (m_verbose) cout << "*** MISSED: Shooting to the other side" << endl;
	}
	else
	{
		Angle ang = in_pose.heading();
		Dist dy = (measures::SIZE_X() - in_pose.x()) * (ang.sin()/ang.cos());
		Dist distFromCenter = in_pose.y() + dy - measures::SIZE_Y()/2;
		//Dist absDistFromCenter = (distFromCenter > 0 ? distFromCenter : -distFromCenter);
		//if (absDistFromCenter < Milim(300) )
		if (distFromCenter.eq(Dist(), Milim(300)))
		{
			if (m_verbose)
				cout << "*** SCORED HIT!!! at " << distFromCenter.mm() << " mm from center" << endl;
			m_score++;
			//m_lastHitTime = m_timeChange.m_time;
		}
		else if (m_verbose)
			cout << "*** MISSED: at "<< distFromCenter.mm() << " mm from center" << endl;
	}
	if (m_verbose)
		cout << "*** Current score is " << m_score << endl; 
		//", current time is " << m_timeChange.m_time.ms() / 1000.0 << " s" << endl;
}
//}}}1

