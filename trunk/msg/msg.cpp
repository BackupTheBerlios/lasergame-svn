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

	struct TaskImplKey : public thread::Key //{{{1
	{
		TaskImpl* operator () () { return (TaskImpl*)getVoid(); }
		using thread::Key::operator = ;
	}	s_impl;

	class TaskImpl : public std::list<TaskItem>, public thread::Lock, public thread::CondVar //{{{1
	{
		public:
			thread::id_t m_id;
			TaskImpl() { s_impl = this; }
			void wait() { thread::CondVar::wait(*this); }	
			void push_back_locked(SubsBase* in_subs, WrappedBase* in_item)
			{
				lock(); 
				push_back(TaskItem(in_subs, in_item)); 
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

	TaskImpl s_main;
	
	class AllSubs : public thread::Lock, public std::map< std::string, Channel* > //{{{1
	{
		public:
			Channel* getChannel(const char* in_name);
	} s_allSubs;
	
	TaskItem getItem() //{{{1
	{
		// 1. find current thread and associated queue
		TaskImpl* t = s_impl();
		// 2. lock it
		Locker lock(*t);
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
		TaskImpl* t = s_impl();
		// 2. lock it
		Locker lock(*t);
		// 3. loop through all and erase & delete items for in_subs (use TaskItemBase::isFor(SubsBase))
		for (TaskImpl::iterator i = t->begin(); i != t->end(); ++i)
		{
			if (i->isFor(in_subs))
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
	ulong threadFn(void* in_helper) //{{{1
	{
		// create support structures for messages
		TaskImpl m;
		// create the user object
		Runnable* dummy = ((HelperBase*)(in_helper))->create();
		// delete in_helper since it is no longer needed
		delete ((HelperBase*)(in_helper));
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

void SubsBase::publish() const //{{{1
{
	Locker lock(s_allSubs);
	for (std::list<msg::SubsBase*>::iterator i = m_pChannel->begin(); i != m_pChannel->end(); i++)
	{
		if (*i != this)
			(*i)->m_taskImpl.push_back_locked(*i, createWrapped()); // thread safe
	}
}

SubsBase::SubsBase() : m_pChannel(new Channel()), m_taskImpl(*s_impl()) //{{{1
{
	// no need to lock since nobody else has access to this newly created channel
	m_pChannel->push_back(this);
}

SubsBase::SubsBase(const char * in_name) : m_pChannel(s_allSubs.getChannel(in_name)), m_taskImpl(*s_impl()) //{{{1
{
	Locker lock(s_allSubs);
	// 1. register this subs
	m_pChannel->push_back(this);
}

SubsBase::SubsBase(Channel* in_pChannel) : m_pChannel(in_pChannel), m_taskImpl(*s_impl()) //{{{1
{
	Locker lock(s_allSubs);
	// 1. register this subs
	m_pChannel->push_back(this);
}


SubsBase::~SubsBase() //{{{1
{
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

void wait() //{{{1
{
	// remove the item from the top of the current task queue, thread safe, blocking
	TaskItem item = getItem();
	// assign the data according to the subscription
	item.assign();   
}
//}}}

Task::Task(HelperBase* in_helper) //{{{1
{
	m_id = thread::create(threadFn, in_helper);
}

Task::~Task() //{{{1
{
	// TODO: tell the thread to stop
	thread::join(m_id);
}
//}}}
} // namespace msg

namespace
{
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
}
