#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED 1

/**
 * @file 
 *     Task definition and documentation.
 *          
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id$
 */

#include <typeinfo>

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

namespace task
{
	class Task
	{
		public:
			Task();
			~Task();
	};
	
	class Cond
	{
		public:
			virtual operator bool() const = 0; 
			virtual ~Cond() {}
	};

	class True : public Cond
	{
		public:
			virtual operator bool() const { return true; }
	};

	/// Awaits message of type in_type
	void wait(const std::type_info& in_type);
	/// Awaits for condition in_cond to become true
	void wait(Cond& in_cond);
	/// Awaits a single message
	void wait();
	
	Task& current();
}

#endif // TASK_H_INCLUDED

