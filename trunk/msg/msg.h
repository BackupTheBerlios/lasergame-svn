#ifndef MSG_H_INCLUDED
#define MSG_H_INCLUDED 1

/**
 * @file 
 *     Message sending & receiving infrastracture
 *          
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id$
 */

/**
 * Task represents a separate thread of execution. Tasks will communicate 
 * by sending messages to each other. There are at least 3 posibilities how 
 * this could work:
 * 
 * 	# publish/subscribe - Task has sources and sinks, when a Task produces
 * 		some data it gets to anyone interested (subscribed). In this model the
 * 		sender has no idea to whom it is sending. It just publishes some data.
 * 		This arangement could be used for Tasks of equal importance/independence.
 * 		Needs some name services. Would allow at least two ways of identification
 * 		of incomming messages:
 * 			o by data type (coarser)
 * 			o by subscription (finer)
 * 		Either handlers or data storage can be registered as the target of the messages.
 *
 * 	# send/? - Task can send a message to another Task. Here the sender needs
 * 		some way of identification of the recepient (ie. send only to its parent).
 * 		This arangement would favor parent-child communication.
 * 		Does not need name services, but would help. Allows identification of incomming
 * 		messages only by type. That means that a Task receiving msg::Pose needs
 * 		some other means of identifying what it is Pose of.
 *
 * 	# request/reply - This could be tricky to implement but sometimes it could
 * 		be worth it. I am not sure. What to do when a Task is waiting for a reply
 * 		and is commanded to stop? etc.
 *
 * 	# streams (connectors) - Two way communication.
 *
 * 	Going again over the Robocode API I am still not sure what to think of the
 * 	possibility of calling a blocking API from inside of a message handler
 * 	(related to the message priorities etc.). Should I go ahead and program some
 * 	controllers for Robocode? Hmm...
 *
 * 	However, the fastest way how to proceed forward is to implement publish/subscribe
 * 	protocol with "by subscription" identification and "data storage" registration
 * 	because that is very closely related to what is already implemented for Ester
 * 	in Eurobot 2004 (Sirael Team).
 */

#include "util/thread.h"

namespace msg
{
	/// Class for a computational context (can accept messages)
	class TaskImpl;
	
	class Runnable { public: virtual void main() = 0; virtual ~Runnable() {} };
	
	class HelperBase { public: TaskImpl* m_taskImpl; virtual Runnable* create() = 0; };

	template <class What, class Par1> class Helper : public HelperBase
	{
		Par1 m_par1;
		public:
			Helper(const Par1 in_par1) : m_par1(in_par1) {}
			virtual Runnable* create() { return new What(m_par1); }
	};

	template <class What, class Par1> HelperBase* helper(const Par1 in_par1)
	{
		return new Helper<What, Par1>(in_par1);
	}

#ifndef JOIN
#	define JOIN( X, Y ) DO_JOIN( X, Y )
#	define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#	define DO_JOIN2( X, Y ) X##Y
#endif

#	define TASK(a,b) Task JOIN(unique,__LINE__)(helper<a>(b))

	class Task
	{
		thread::id_t m_id;
		TaskImpl* m_pimpl;
		public:
			/// creates thread
			Task(HelperBase* in_helper);
			/// waits for its thread to finish
			~Task();
	};

	class SubsBase;
	class WrappedBase
	{
		public: virtual void assignTo(SubsBase* in_subs) const = 0;
	};

	class Channel;
	
	class SubsBase
	{
		Channel* m_pChannel;
		TaskImpl& m_taskImpl;
		public:
			SubsBase();
			SubsBase(const char* in_name);
			SubsBase(Channel* in_pChannel);
			Channel* getChannel() { return m_pChannel; }
			virtual ~SubsBase();
			virtual WrappedBase* createWrapped() const = 0;
			void publish() const;
	};

	template <class T> class Wrapped : public WrappedBase
	{
		T m_data;
		public:
			Wrapped(const T& in_data) : m_data(in_data) {}
			void assignTo(SubsBase* in_subs) const
			{
				*dynamic_cast<T*>(in_subs) = m_data;
			}
	};

	template <class T> class Subs : public SubsBase, public T
	{
		public:
			Subs() {}
			Subs(const char* in_name) : SubsBase(in_name) {}
			Subs(Channel* in_pChannel) : SubsBase(in_pChannel) {}
			virtual WrappedBase* createWrapped() const
			{
				return new Wrapped<T>(*this);
			}
	};
	
	/// Awaits a single message
	void wait();
	/// Awaits a message for in_subs
	void wait(SubsBase& in_subs);
}

#endif // MSG_H_INCLUDED

