// $Id$
// Copyright (C) 2004, Zbynek Winkler

///@file
#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED 1

#include "field.h"
#include "util/msg.h"
#include "util/conf.h"

#include <vector>

/// Eurobot 2004
class Manager : public msg::Runnable
{
	private:
		Field& m_field;
		const conf::Robot& m_a;
		const conf::Robot& m_b;
		msg::Channel* m_pa;
		msg::Channel* m_pb;

		std::vector<msg::Task*> m_drivers;
		
		void waitForDrivers(msg::Subs<int>& in_done);

	public:
		Manager(Field & in_field, const conf::Robot & in_a, const conf::Robot & in_b, 
				msg::Channel* in_pa, msg::Channel* in_pb);
		~Manager();
		virtual void main();
};

#endif // MANAGER_H_INCLUDED

