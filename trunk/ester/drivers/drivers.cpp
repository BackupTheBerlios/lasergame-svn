// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include "drivers.h"
#include "watchdog.h"
#include "ester-speed.h"
#include "pose-change.h"
#include "pose.h"
#include "floor-color.h"
#include "gp2top.h"
#include "ball-manip.h"
#include "ball-detect.h"
#include "util/assert.h"

#include <map>
#include <string>

namespace drivers {
	
	typedef std::map< std::string, pFac > DriverTable;
	DriverTable s_driverTable;
	
	template <typename T> msg::Runnable* create(Driver::Params& in_params)
	{
		return new T(in_params);
	}
	
	template <typename T> void r(const char* in_name)
	{
		s_driverTable[in_name] = &create<T>;
	}

	class Registrator
	{
		public:
			Registrator()
			{
				// register all drivers with their names
				r<Watchdog>("watchdog");
				r<ester::Speed>("ester-speed");
				r<PoseChange>("pose-change");
				r<Pose>("pose");
				r<GP2Top>("gp2top");
				r<FloorColor>("floor-color");
				r<BallManip>("ball-manip");
				r<BallDetect>("ball-detect");
			}
	} s_reg;
	
	pFac getFactory(const char* in_name)
	{
		ASSERT( s_driverTable.count(in_name) == 1 );
		return s_driverTable[in_name];
	}
}

