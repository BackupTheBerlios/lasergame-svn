// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef DRIVERS_POSE_CHANGE_H_INCLUDED
#define DRIVERS_POSE_CHANGE_H_INCLUDED 1

#include "drivers.h"
#include "number/time.h"
#include "number/speed.h"
#include "number/pose.h"

namespace drivers {

class PoseChange : public Driver
{
	public:
		PoseChange(msg::Channel* in_p, int in_id, msg::Channel* in_done) : Driver(in_p, in_id, in_done) {}
		virtual void main()
		{
			using namespace msg;
			using namespace num;
			using namespace std;
			Subs< pair<Time, Speed> > in(m_p, "time-change+speed-current");
			Subs< Pose > poseChange(m_p, "pose-change");
			Subs<int>    done(m_done);

			done.value = m_myID;
			done.publish();
			
			while (true)
			{
				waitFor(in);
				Time& dt(in.value.first);
				Speed& speed(in.value.second);
				poseChange.value = Pose(speed.m_forward * dt, speed.m_side * dt, speed.m_angular * dt);
				poseChange.publish();
				
				done.value = m_myID;
				done.publish();
			}
		}
};

}

// TODO
//
// 1) predelat Subs na Input a Output, aby se u veci, co pouze generuji data, to tam nepletlo 
// (viz "done") -- asi nehori
// 
// 2) odpovedet novinky.cz -- done
//
// 3) odpovedet Nova, Akademie ved
// 
// 4) jak udelat Shoot? Potrebuje request od uzivatele (asynchronni), aktualni pozici a 
// jako vstup vystup in-balls... to uz ma ale jako vystup ten, co micky pozira... :-(
// Mozna sloucit Shoot a InBalls???
//
// 5) udelat filtrovane rnd???

#endif // DRIVERS_POSE_CHANGE_H_INCLUDED

