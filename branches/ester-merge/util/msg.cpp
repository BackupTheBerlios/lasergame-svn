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
			void* m_user;
		
		public:
			TaskItem(SubsBase* in_targetSubs, void* in_user) 
				: m_subs(in_targetSubs), m_user(in_user) {}
			/// Intentionaly blank 
			~TaskItem() {}
			SubsBase* getSubs() { return m_subs; }
			void destroy() { m_subs->destroy(m_user); m_user = 0; }
			void execute() { m_subs->execute(m_user); destroy(); }
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
			void push_back_locked(SubsBase* in_subs, void* in_item)
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
	
	class Channel : public thread::Lock //{{{1
	{
		protected:
			typedef std::map<std::string, Channel*> Children;
			typedef std::list<SubsBase*> SubsList;
		private:
			SubsList m_subs;
			Children m_children;
		public:
			virtual ~Channel() {}
			bool empty() 
			{ 
				return m_subs.empty() && m_children.empty(); 
			}
			void connect(SubsBase* in_subs) 
			{
				if (!m_subs.empty())
					m_subs.front()->checkType(in_subs); // throws
				m_subs.push_back(in_subs); 
			}
			void disconnect(SubsBase* in_subs)
			{
				m_subs.erase(std::find(m_subs.begin(), m_subs.end(), in_subs));
				// if that was the last subs under the group and there are no subgroups, delete it
				if (empty())
					delete this;
			}
			void erase(Children::iterator in_child)
			{
				m_children.erase(in_child);
			}
			Channel* getChannel(const char* in_name);
			void publish(const SubsBase* in_subs)
			{
				for (SubsList::iterator i = m_subs.begin(); i != m_subs.end(); i++)
				{
					if (*i != in_subs)
						(*i)->postItem(in_subs->create()); // locks SubsBase::taskImpl
				}
			}
	};

	class BoundChannel : public Channel //{{{1
	{
			friend class Channel;
			Channel* m_parent;
			Children::iterator m_self;
		public:
			BoundChannel(Channel* in_parent) : m_parent(in_parent), m_self(0) { }
			virtual ~BoundChannel()
			{
				ASSERT( m_parent != 0 );
				ASSERT( m_self != Children::iterator(0) );
				m_parent->erase(m_self);
			}
	};

	Channel* Channel::getChannel(const char* in_name) //{{{1
	{
		Children::iterator ret = m_children.lower_bound(in_name);
		if (ret == m_children.end() || (*ret).first != in_name)
		{
			BoundChannel* bc = new BoundChannel(this);
			ret = m_children.insert(ret, Children::value_type(in_name, bc));
			bc->m_self = ret;
		}
		return ret->second;
	}

	SubsBase::SubsBase() : m_pChannel(0), m_taskImpl(*s_impl()) //{{{1
	{
	}

	void SubsBase::subscribe() //{{{1
	{
		ASSERT( m_pChannel == 0 );
		m_pChannel = new Channel();
		m_pChannel->connect(this);
	}

	void SubsBase::subscribe(Channel* in_parent, const char * in_name) //{{{1
	{
		using namespace std;
		ASSERT( m_pChannel == 0 );
		Locker lock(*in_parent);
		m_pChannel = in_parent->getChannel(in_name);
		m_pChannel->connect(this);
	}

	void SubsBase::subscribe(Channel* in_pChannel) //{{{1
	{
		ASSERT( m_pChannel == 0 );
		Locker lock(*in_pChannel);
		m_pChannel = in_pChannel;
		m_pChannel->connect(this);
	}

	void SubsBase::unsubscribe() //{{{1
	{
		ASSERT( m_pChannel != 0 );
		Locker lock(*m_pChannel);
		m_pChannel->disconnect(this);
	}

void SubsBase::publish() const //{{{1
{
	Locker lock(*m_pChannel);
	m_pChannel->publish(this);
}

void SubsBase::postItem(void* in_data)
{
	m_taskImpl.push_back_locked(this, in_data); // thread safe
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
	t->front().execute();
	t->pop_front();
	return ret;
	// [4. auto-unlock]
}

void waitFor(SubsBase& in_subs) //{{{1
{
	while (processSubs() != &in_subs)
		;
}
namespace detail {
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

