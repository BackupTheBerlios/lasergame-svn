// vim: set fdm=marker:

#include "msg/msg.h"
#include "msg/Task.h"

namespace msg {
	
#if 0
class Queue
{
	public:
		static Queue& get(const char * in_name);
};

Message::Message(const char * in_name) : m_task(Task::getCurrent()), m_queue(Queue::get(in_name)) //{{{1
{
	// TODO
}

Message::Message(const Message& in_msg) : m_task//{{{1
{
}

Message::~Message() //{{{1
{
	// TODO
}
//}}}
#endif

} // namespace msg

