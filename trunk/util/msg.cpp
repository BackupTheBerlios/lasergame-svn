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
 * All manipulation with channels and subscriptions is guarded by a single
 * lock s_allSubs. Task queues have their own locks.
 */

#include <map>
#include <list>
#include <string>
#include <algorithm>
//#include <iostream>
#include "util/thread.h"
#include "util/assert.h"
#include "util/msg.h"

namespace msg 
{ 
	class TaskItem //{{{1
	{
		protected:
			SubsBase* m_subs;
			WrappedBase* m_user;
		
		public:
			TaskItem(SubsBase* in_targetSubs, WrappedBase* in_user) 
				: m_subs(in_targetSubs), m_user(in_user) {}
			/// Intentionaly blank 
			~TaskItem() {}
			SubsBase* getSubs() { return m_subs; }
			void destroy() { delete m_user; m_user = 0; }
			void assign() { m_user->assignTo(m_subs); destroy(); }
	};

	struct TaskImplKey : public thread::Key //{{{1
	{
		TaskImpl* operator () () { return (TaskImpl*)getVoid(); }
		using thread::Key::operator = ;
	}	s_impl;

	class TaskImpl : public std::list<TaskItem>, public thread::Lock, public thread::CondVar //{{{1
	{
		friend class Task;
		thread::id_t m_thread;
		bool m_running;
		public:
			TaskImpl() : m_running(true) { }
			~TaskImpl() { ASSERT( empty() ); }
			void wait() { thread::CondVar::wait(*this); }	
			bool running() { return m_running; }
			void push_back_locked(SubsBase* in_subs, WrappedBase* in_item)
			{
				lock(); 
				push_back(TaskItem(in_subs, in_item)); 
				broadcast(); 
				unlock();
			}
			void stop()
			{
				lock();
				m_running = false;
				broadcast();
				unlock();
			}
	};
	//}}}
}

namespace 
{
	using namespace msg;
	using namespace thread;

	class Main : public TaskImpl //{{{1
	{
		public: Main() { s_impl = this; }
	} s_main;

	class stop {};	//{{{1
	
	class AllSubs : public thread::Lock, public std::map< std::string, Channel* > //{{{1
	{
		public:
			Channel* getChannel(const char* in_name);
	} s_allSubs;
	
	void destroyItemsFor(SubsBase* in_subs) //{{{1
	{
		// 1. find current thread and associated queue
		TaskImpl* t = s_impl();
		// 2. lock it
		Locker lock(*t);
		// 3. loop through all and erase & delete items for in_subs (use TaskItemBase::isFor(SubsBase))
		for (TaskImpl::iterator i = t->begin(); i != t->end(); ++i)
		{
			if (i->getSubs() == in_subs)
			{
				TaskImpl::iterator to_del = i;
				i++;
				to_del->destroy();
				t->erase(to_del);
				i--;
			}
		}
		// [4. auto-unlock]
	}
	ulong threadFn(void* in_void) //{{{1
	{
		FactoryBase* in_factory = (FactoryBase*)(in_void);
		// set thread specific variable s_impl to point to the support class for this thread
		s_impl = in_factory->m_taskImpl;
		// create the user object
		Runnable* dummy = in_factory->create();
		// delete in_Factory since it is no longer needed
		delete in_factory;
		// call user supplied main
		try { dummy->main(); } catch(...) {}
		// delete the user object
		delete dummy;
		return 0;
	}
	//}}}
}

namespace msg {
	
	class Channel : public std::list<SubsBase*> //{{{1
	{
		public:
			AllSubs::iterator m_self;
			Channel() : m_self(0) {}
	};

	SubsBase::SubsBase() : m_pChannel(0), m_taskImpl(*s_impl()) //{{{1
	{
	}

	void SubsBase::subscribe() //{{{1
	{
		ASSERT( m_pChannel == 0 );
		m_pChannel = new Channel();
		m_pChannel->push_back(this);
	}

	void SubsBase::subscribe(const char * in_name) //{{{1
	{
		using namespace std;
		ASSERT( m_pChannel == 0 );
		Locker lock(s_allSubs);
		m_pChannel = s_allSubs.getChannel(in_name);
		if (!m_pChannel->empty())
			m_pChannel->front()->checkType(this);
		m_pChannel->push_back(this);
	}

	void SubsBase::subscribe(Channel* in_pChannel) //{{{1
	{
		ASSERT( m_pChannel == 0 );
		Locker lock(s_allSubs);
		m_pChannel = in_pChannel;
		// check type
		if (!m_pChannel->empty())
			m_pChannel->front()->checkType(this);
		m_pChannel->push_back(this);
	}

	SubsBase::~SubsBase() //{{{1
	{
		ASSERT( m_pChannel != 0 );
		{
			Locker lock(s_allSubs);
			// 1. deregister this subs
			m_pChannel->erase(std::find(m_pChannel->begin(), m_pChannel->end(), this));
			// 2. if this was the last subs under the group, delete it
			if (m_pChannel->empty())
			{
				if (m_pChannel->m_self != AllSubs::iterator(0))
					s_allSubs.erase(m_pChannel->m_self);
				delete m_pChannel;
			}
		}
		// 3. go through the task queue and delete all messages destined to this subs
		destroyItemsFor(this);
	}

void SubsBase::publish() const //{{{1
{
	Locker lock(s_allSubs);
	for (std::list<msg::SubsBase*>::iterator i = m_pChannel->begin(); i != m_pChannel->end(); i++)
	{
		if (*i != this)
			(*i)->m_taskImpl.push_back_locked(*i, createWrapped()); // thread safe
	}
}

// remove the item from the top of the current task queue, thread safe, blocking
// assign the data according to the subscription
SubsBase* processSubs() //{{{1
{
	// 1. find current thread and associated queue
	TaskImpl* t = s_impl();
	// 2. lock it
	Locker lock(*t);
	// 3. pop and return the item
	while (t->empty() && t->running())
		t->wait();
	if (!t->running())
		throw stop();
	SubsBase* ret = t->front().getSubs();
	t->front().assign();
	t->pop_front();
	return ret;
	// [4. auto-unlock]
}

void waitFor(SubsBase& in_subs) //{{{1
{
	while (processSubs() != &in_subs)
		;
}
//}}}

Task::Task(FactoryBase* in_factory) //{{{1
{
	// create support structures for messages
	m_pimpl = new TaskImpl();
	in_factory->m_taskImpl = m_pimpl;
	m_pimpl->m_thread = thread::create(threadFn, in_factory);
}

Task::~Task() //{{{1
{
	m_pimpl->stop();
	thread::join(m_pimpl->m_thread);
	delete m_pimpl;
}
//}}}
} // namespace msg

	Channel* AllSubs::getChannel(const char* in_name) //{{{1
	{
		iterator ret = lower_bound(in_name);
		if (ret == end() || (*ret).first != in_name)
		{
			ret = insert(ret, value_type(in_name, new Channel()));
			(*ret).second->m_self = ret;
		}
		return ret->second;
	}	//}}}