// vim: set fdm=marker:

#include <map>
#include <list>
#include <string>
#include <algorithm>
#include "util/thread.h"
#include "msg/msg.h"

namespace msg { class SubsList : public std::list<msg::SubsBase*> { }; }

namespace 
{
	thread::Lock s_lock;
	std::map< std::string, msg::SubsList > s_allSubs;
	
	class Locker : public thread::Locker
	{
		public:
			Locker() : thread::Locker(s_lock) {}
	};

	msg::TaskItemBase* getItem()
	{
		// 1. find current thread and associated queue
		// 2. lock it
		// 3. pop and return the item
		// [4. auto-unlock]
		return 0;
	}
}

namespace msg {

void SubsBase::publish() const
{
	Locker lock;
	for (std::list<msg::SubsBase*>::iterator i = m_subList.begin(); i != m_subList.end(); i++)
	{
	//	if (*i != this)
	//		i->m_task.appendItem(createTaskItem(i));
	//		or
	//		pushItem(i->m_thread, createTaskItem(i)); // thread safe
	}
}

SubsBase::SubsBase(const char * in_name) : m_subList(s_allSubs[in_name])
{
	Locker lock;
	// 1. register this subs
	m_subList.push_back(this);
}

SubsBase::~SubsBase()
{
	Locker lock;
	// 1. deregister this subs
	m_subList.erase(std::find(m_subList.begin(), m_subList.end(), this));
	// 2. if this was the last subs under the group, delete it
	// TODO
	// 3. go through the task queue and delete all messages destined to this subs
	// TODO
}

void wait()
{
	// remove the item from the top of the current task queue, thread safe, blocking
	TaskItemBase* item = getItem();
	// assign the data according to the subscription
	//item->assign();   
	delete item;
}

} // namespace msg

namespace
{

} // anonymous namespace
