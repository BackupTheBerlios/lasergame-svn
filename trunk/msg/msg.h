#ifndef MSG_H_INCLUDED
#define MSG_H_INCLUDED 1

/**
 * @file 
 *     Message declaration and documentation.
 *          
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id$
 */

#include "msg/task.h"

namespace msg
{
	/// Abstract class for all messages
	class Message
	{
		public:
			/// Empty, to ensure proper deletition.
			virtual ~Message() {}
	};

	class Subs
	{
		public:
			static Subs& get(const char * in_name) { static Subs s; return s; }
			void remove(Message *) {}
	};

	template <class T> class MessageImpl : public T
	{
		private:
			/// Parent Task
			task::Task& m_task;
			/// 
			Subs& m_subs;
		
		public:
			/// Subscribes itself to the in_name
			MessageImpl(const char* in_name) : m_task(task::current()), m_subs(Subs::get(in_name))
			{
			}
			
			/// Unsubscribes itself
			virtual ~MessageImpl()
			{
				m_subs.remove(this);
			}
			
			/// Sends out a copy of itself to all members of the same Queue
			void publish() const
			{
#if 0
				for (Subs::iter i = m_subs.begin(); i != m_subs.end(); i++)
				{
					if (*i != this)
						i->m_task.enqueueMessage(new T(*this));
				}
#endif
			}
	};
}

#endif // MSG_H_INCLUDED

