#include "field.h"

	using namespace measures;
	ASSERT( m_side == 0 || m_side == 1 );
	if (in_flags & RAND_PALM) randPalm(0); else setPalm(2,2);
	if (in_flags & RAND_BALL)	setBall(0); else setBall(2,6,11);
	if (in_flags & ERROR)	setError();

void EsterSimul::setBall(uint32_t in_seed) //{{{1
{
	using namespace measures;
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

void EsterSimul::randPalm(uint32_t in_seed) //{{{1
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

void EsterSimul::setBall(int in_pos1, int in_pos2, int in_pos3) //{{{1
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
		if ((position.x*TILE()).eq(m_palmPosition1.x()) && (position.y*TILE()).eq(m_palmPosition1.y()))
			continue; // skip position where are the palms
		cards[curCard++] = position;
	}
	ASSERT( curCard == BALL_POSITIONS - 2 );

	m_balls.clear();

	m_balls.push_back(Place(measures::TILE() * 2, measures::TILE() * 5 ));;
	m_balls.push_back(Place(measures::TILE() * (8 - 2) , 
													measures::TILE() * (7 - 5)));

	m_balls.push_back(Place(measures::TILE() * cards[in_pos1].x, measures::TILE() * cards[in_pos1].y ));
	m_balls.push_back(Place(measures::TILE() * (8 - cards[in_pos1].x) , 
													measures::TILE() * (7-cards[in_pos1].y )));

	m_balls.push_back(Place(measures::TILE() * cards[in_pos2].x, measures::TILE() * cards[in_pos2].y ));
	m_balls.push_back(Place(measures::TILE() * (8 - cards[in_pos2].x) , 
													measures::TILE() * (7-cards[in_pos2].y )));

	m_balls.push_back(Place(measures::TILE() * cards[in_pos3].x, measures::TILE() * cards[in_pos3].y ));
	m_balls.push_back(Place(measures::TILE() * (8 - cards[in_pos3].x) , 
													measures::TILE() * (7-cards[in_pos3].y )));
}

void EsterSimul::setPalm(int in_crossingX, int in_crossingY) //{{{1
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
	m_palmPosition1 = Place(measures::TILE() * in_crossingX, measures::TILE() * in_crossingY );
	m_palmPosition2 = Place(measures::TILE() * (8 - in_crossingX) , 
			                    measures::TILE() * (7 - in_crossingY));
}

void EsterSimul::printStat() //{{{1
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

void EsterSimul::printIntro() //{{{1
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

void EsterSimul::checkPalms() //{{{1
{
	if (m_pose.m_pose.distanceTo(m_palmPosition1) < m_minPalmDist)
	{
		m_minPalmDist = m_pose.m_pose.distanceTo(m_palmPosition1);
		m_minPalmPose = m_pose.m_pose;
		m_minPalmTime = m_timeChange.m_time;
	}
	if (m_pose.m_pose.distanceTo(m_palmPosition2) < m_minPalmDist)
	{
		m_minPalmDist = m_pose.m_pose.distanceTo(m_palmPosition2);
		m_minPalmPose = m_pose.m_pose;
		m_minPalmTime = m_timeChange.m_time;
	}
}

void EsterSimul::checkEnemy() //{{{1
{
  if (!m_checkEnemy)
    return ;
  if (m_pose.m_pose.distanceTo(m_enemyPosition) < m_minEnemyDist)
  {
	  m_minEnemyDist = m_pose.m_pose.distanceTo(m_palmPosition1);
	  m_minEnemyPose = m_pose.m_pose;
	  m_minEnemyTime = m_timeChange.m_time;
  }
}

