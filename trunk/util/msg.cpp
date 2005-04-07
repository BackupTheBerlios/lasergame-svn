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

namespace { ulong threadFn(void* in_void); }

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
			/// Intentionaly blank -- cannot destroy m_user data because it wouldn't be copy-safe
			~TaskItem() {}
			SubsBase* getSubs() { return m_subs; }
			void destroyUser() { m_subs->destroy(m_user); m_user = 0; }
			void execute() { m_subs->accept(m_user); destroyUser(); }
	};

	struct TaskImplKey : public thread::Key //{{{1
	{
		TaskImpl* operator () () { return (TaskImpl*)getVoid(); }
		using thread::Key::operator = ;
	}	s_impl;

	class TaskImpl : public std::list<TaskItem>, public thread::Lock, public thread::CondVar //{{{1
	{
		public:
			thread::id_t m_thread;
			bool m_running;
			Runnable* m_runnable;
		
		public:
			TaskImpl() : m_running(true), m_runnable(0) { }
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
		TaskImpl* in_taskImpl = (TaskImpl*)(in_void);
		// set thread specific variable s_impl to point to the support class for this thread
		s_impl = in_taskImpl;
		// call user supplied main
		try { in_taskImpl->m_runnable->main(); } 
		catch(stop) {} 
		catch(...) { ASSERT( false ); }
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
			SubsList m_subs;     // destinations
			Children m_children; // sub-channels
			int m_numOut;        // pure putputs
		public:
			Channel() : m_numOut(0) {}
			virtual ~Channel() {}
			bool empty() 
			{ 
				return m_subs.empty() && m_children.empty() && m_numOut == 0 ; 
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
				// if that was the last child channel and there are no subscription, delete it
				if (empty())
					delete this;
			}
			Channel* getChannel(const char* in_name);
			void publish(const SubsBase* in_subs)
			{
				for (SubsList::iterator i = m_subs.begin(); i != m_subs.end(); i++)
				{
					if (*i != in_subs) // do not publish to the calling subs
						(*i)->postItem(in_subs->create()); // locks SubsBase::taskImpl
				}
			}
#if 0
			// will need two params - who's outputing and where to not put it
			void publish()
			{
				for (SubsList::iterator i = m_subs.begin(); i != m_subs.end(); i++)
					(*i)->postItem(in_subs->create()); // locks SubsBase::taskImpl
			}
#endif
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

	SubsBase::SubsBase() : m_pChannel(0), m_taskImpl(0) //{{{1
	{
	}
	
	SubsBase::operator Channel* () //{{{1
	{
		ASSERT( m_pChannel != 0 );
		return m_pChannel;
	}

	void SubsBase::subscribe(Channel* in_pChannel) //{{{1
	{
		ASSERT( m_taskImpl == 0 && m_pChannel == 0 );
		m_taskImpl = s_impl();
		if (in_pChannel == 0) in_pChannel = new Channel();
		Locker lock(*in_pChannel);
		m_pChannel = in_pChannel;
		m_pChannel->connect(this);
	}

	void SubsBase::subscribe(Channel* in_parent, const char * in_name) //{{{1
	{
		using namespace std;
		ASSERT( m_taskImpl == 0 && m_pChannel == 0 );
		m_taskImpl = s_impl();
		Locker lock(*in_parent);
		m_pChannel = in_parent->getChannel(in_name);
		m_pChannel->connect(this);
	}

	void SubsBase::unsubscribe() //{{{1
	{
		ASSERT( m_taskImpl != 0 && m_pChannel != 0 );
		Locker lock(*m_pChannel);
		m_pChannel->disconnect(this);
	}

	void SubsBase::publish() const //{{{1
	{
		ASSERT( m_taskImpl != 0 && m_pChannel != 0 );
		Locker lock(*m_pChannel);
		m_pChannel->publish(this);
	}

void SubsBase::postItem(void* in_data) //{{{1
{
	m_taskImpl->push_back_locked(this, in_data); // thread safe
}
// remove the item from the top of the current task queue, thread safe, blocking
// assign the data according to the subscription
SubsBase* processSubs() //{{{1
{
	// 1. find current thread and associated queue
	TaskImpl* t = s_impl();
	// 2. lock it
	t->lock();
	// 3. pop and return the item
	while (t->empty() && t->running())
		t->wait();
	if (!t->running())
	{
		// [4. unlock]
		t->unlock();
		throw stop();
	}
	TaskItem ti(t->front());
	t->pop_front();
	// [4. unlock]
	t->unlock();
	ti.execute(); // destroys user data
	return ti.getSubs();
}

void waitFor(SubsBase& in_subs) //{{{1
{
	ASSERT( in_subs.m_pChannel != 0 && in_subs.m_taskImpl != 0 );
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
				to_del->destroyUser();
				t->erase(to_del);
				i--;
			}
		}
		// [4. auto-unlock]
	}
//}}}
}

Task::Task(Runnable* in_runnable) //{{{1
{
	m_delete = true;
	m_pimpl = new TaskImpl();
	m_pimpl->m_runnable = in_runnable;
	m_pimpl->m_thread = thread::create(threadFn, m_pimpl);
}

Task::Task(Runnable& in_runnable) //{{{1
{
	m_delete = false;
	m_pimpl = new TaskImpl();
	m_pimpl->m_runnable = &in_runnable;
	m_pimpl->m_thread = thread::create(threadFn, m_pimpl);
}

Task::~Task() //{{{1
{
	m_pimpl->stop();
	thread::join(m_pimpl->m_thread);
	if (m_delete) delete m_pimpl->m_runnable;
	delete m_pimpl;
}
//}}}
} // namespace msg

