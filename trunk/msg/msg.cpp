// vim: set fdm=marker:

/**
 * @file 
 *     Message sending & receiving infrastracture (implementation)
 *
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id$
 *
 * Messages are sent and delivered through subscriptions Subs. Subs are identified
 * by a name (const char*) and stored in a std::map. Each item in the map is a list
 * of Subs that were initialized to the same name. To produce data a client calls
 * Subs::publish() method that posts TaskItems to the task queues of the subscriptions
 * initialized to the same name.
 *
 * All manipulation with any data that could possibly be shared is guarded by a single
 * lock s_lock available only for this and only file. Helper class Locker is provided
 * to automaticaly lock and unlock this lock. Only Task queues have their own locks.
 * 
 */

#include <map>
#include <list>
#include <string>
#include <algorithm>
#include "util/thread.h"
#include "msg/msg.h"

namespace msg 
{ 
	class SubsList : public std::list<SubsBase*> { }; 
	class TaskItem //{{{1
	{
		protected:
			SubsBase* m_subs;
			WrappedBase* m_user;
		
		public:
			TaskItem(SubsBase* in_targetSubs, WrappedBase* in_user) 
				: m_subs(in_targetSubs), m_user(in_user) {}
			bool isFor(SubsBase* in_subs) { return m_subs == in_subs; }
			void destroy() { delete m_user; m_user = 0; }
			void assign() { m_user->assignTo(m_subs); destroy(); }
	};

	class TaskBase::Impl : public std::list<TaskItem>, public thread::Lock, public thread::CondVar
	{
		public:
			void wait() { thread::CondVar::wait(*this); }	
			void push_back_locked(SubsBase* in_subs, WrappedBase* in_item)
			{
				lock(); push_back(TaskItem(in_subs, in_item)); broadcast(); unlock();
			}
	};

	TaskBase::TaskBase()
	{
		m_pimpl = new Impl();
	}

	TaskBase::~TaskBase()
	{
		delete m_pimpl;
	}

	struct TaskBaseImplKey : public thread::Key //{{{1
	{
		TaskBase::Impl* operator () () { return (TaskBase::Impl*)getVoid(); }
		using thread::Key::operator = ;
	}	s_impl;

}

namespace 
{
	using namespace msg;

	class Main : public TaskBase //{{{1
	{
		public:
			Main() { s_impl = this->m_pimpl; }
	} s_main;
	
	class AllSubs : public thread::Lock, public std::map< std::string, SubsList > //{{{1
	{ } s_allSubs;
	
	class Locker : public thread::Locker //{{{1
	{
		public:
			Locker() : thread::Locker(s_allSubs) {}
	};

	TaskItem getItem() //{{{1
	{
		// 1. find current thread and associated queue
		TaskBase::Impl* t = s_impl();
		// 2. lock it
		thread::Locker lock(*t);
		// 3. pop and return the item
		while (t->empty())
			t->wait();
		TaskItem i(t->front());
		t->pop_front();
		return i;
		// [4. auto-unlock]
	}

	void destroyItemsFor(SubsBase* in_subs) //{{{1
	{
		// 1. find current thread and associated queue
		TaskBase::Impl* t = s_impl();
		// 2. lock it
		thread::Locker lock(*t);
		// 3. loop through all and erase & delete items for in_subs (use TaskItemBase::isFor(SubsBase))
		for (TaskBase::Impl::iterator i = t->begin(); i != t->end(); ++i)
		{
			if (i->isFor(in_subs))
			{
				i->destroy();
				t->erase(i);
			}
		}
		// [4. auto-unlock]
	}
	//}}}
}

namespace msg {

void SubsBase::publish() const //{{{1
{
	Locker lock;
	for (std::list<msg::SubsBase*>::iterator i = m_subList.begin(); i != m_subList.end(); i++)
	{
		if (*i != this)
			(*i)->m_taskImpl.push_back_locked(*i, createWrapped()); // thread safe
	}
}

SubsBase::SubsBase(const char * in_name) : m_subList(s_allSubs[in_name]), m_taskImpl(*s_impl()) //{{{1
{
	Locker lock;
	// 1. register this subs
	m_subList.push_back(this);
}

SubsBase::~SubsBase() //{{{1
{
	Locker lock;
	// 1. deregister this subs
	m_subList.erase(std::find(m_subList.begin(), m_subList.end(), this));
	// 2. if this was the last subs under the group, delete it
	// TODO
	// 3. go through the task queue and delete all messages destined to this subs
	destroyItemsFor(this);
}

void wait() //{{{1
{
	// remove the item from the top of the current task queue, thread safe, blocking
	TaskItem item = getItem();
	// assign the data according to the subscription
	item.assign();   
}
//}}}

} // namespace msg

