// $Id$
// Copyright (C) 2004, Zbynek Winkler

#include "manager.h"
#include "drivers/drivers.h"
#include "util/assert.h"

#include <ctime>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace num;
using namespace msg;

Manager::Manager(Field & in_field, const conf::Robot & in_a, const conf::Robot & in_b,  //{{{1
		msg::Channel* in_pa, msg::Channel* in_pb) 
	: m_field(in_field), m_a(in_a), m_b(in_b), m_pa(in_pa), m_pb(in_pb)
{
}

Manager::~Manager() //{{{1
{
	for ( ; !m_drivers.empty(); m_drivers.pop_back())
		delete m_drivers.back();
}

void Manager::main() //{{{1
{
	Subs<int>  driverDone(0);
	
	int i = 0;
	for (conf::Robot::const_iterator iter = m_a.begin() ; iter != m_a.end(); i++,iter++)
	{
		drivers::pFac fac = drivers::getFactory(iter->c_str());
		drivers::Driver::Params p(m_pa, i, 0, driverDone, m_field);
		m_drivers.push_back(new Task(fac(p)));
	}

	for (conf::Robot::const_iterator iter = m_b.begin() ; iter != m_b.end(); i++,iter++)
	{
		drivers::pFac fac = drivers::getFactory(iter->c_str());
		drivers::Driver::Params p(m_pb, i, 1, driverDone, m_field);
		m_drivers.push_back(new Task(fac(p)));
	}

	//cout << "Waiting for drivers to finish initialization..." << endl;
	waitForDrivers(driverDone);
	//cout << "done." << endl;

	Subs<Time> dta(m_pa, "time-change");
	Subs<Time> dtb(m_pb, "time-change");
	while (true)
	{
		dta.value = MSec(5);
		dta.publish();
		dtb.value = MSec(5);
		dtb.publish();
		
		waitForDrivers(driverDone);
		
		// uses internal m_pose[2] set by pose driver, gathers statistics
		m_field.updateStat();
		//m_field.checkPalms();
		//m_field.checkEnemy();
	}
}

void Manager::waitForDrivers(Subs<int>& in_done) //{{{1
{
	int count = m_drivers.size();
	std::vector<bool> done(m_drivers.size(), false);
	while (count > 0)
	{
		//cout << "Waiting... " << endl;
		waitFor(in_done);
		ASSERT( done[in_done.value] == false );
		done[in_done.value] = true;
		count--;
	}
}
//}}}

