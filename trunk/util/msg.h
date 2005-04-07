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

namespace msg
{
	class TaskImpl;
	
	class Runnable { public: virtual void main() = 0; virtual ~Runnable() {} };
	
	class Task //{{{1
	{
		TaskImpl* m_pimpl;
		bool m_delete;
		public:
			/// creates thread
			Task(Runnable* in_runnable);
			Task(Runnable& in_runnable);
			/// waits for its thread to finish
			~Task();
	}; //}}}

	class Channel;
	class bad_type {};
	class SubsBase //{{{1
	{
		Channel* m_pChannel;
		TaskImpl* m_taskImpl;
		friend void waitFor(SubsBase&);
		protected:
//			void subscribe();
			void unsubscribe();
		public:
			SubsBase();
			void subscribe(Channel* in_parent, const char* in_name);
			void subscribe(Channel* in_pChannel);
			virtual ~SubsBase() { }
			virtual void* create() const = 0;
			virtual void destroy(void* in_data) const = 0;
			virtual void checkType(SubsBase*) const = 0;
			virtual void accept(void*) = 0;
			void publish() const;
			operator Channel* ();
			void postItem(void* data);
	};//}}}1

	namespace detail {
		/// Go through the task queue and delete all messages destined to the supplied subs
		void destroyItemsFor(SubsBase* in_subs);
	}

	template <class T> class SubsImpl : public SubsBase //{{{1
	{
		private:
			SubsImpl(const SubsImpl&) {}
		public:
			T value;
			SubsImpl() { /* !!! do not subscribe by default !!! */ }
			SubsImpl(Channel* in_parent, const char* in_name) { subscribe(in_parent, in_name); }
			SubsImpl(Channel* in_pChannel) { subscribe(in_pChannel); }
			virtual ~SubsImpl() { unsubscribe(); detail::destroyItemsFor(this); }
			virtual void* create() const { return new T(value); }
			virtual void destroy(void* in_data) const { delete (T*)in_data; }
			virtual void checkType(SubsBase* in_subs) const
			{
				if (dynamic_cast<SubsImpl<T>*>(in_subs) == 0)
					throw bad_type();
			}
	};

	class UnusedTag {}; 

	template <class T, class C = UnusedTag> class Subs : public SubsImpl<T> //{{{1
	{
		public:
			typedef void (C::*callback)(T&);
			C* m_obj;
			const callback m_c;
		public:
			Subs(C* in_obj, callback in_c) : m_obj(in_obj), m_c(in_c) { /* not subscribed */ }
			Subs(Channel* in_parent, const char* in_name, C* in_obj, callback in_c) 
				: SubsImpl<T>(in_parent, in_name), m_obj(in_obj), m_c(in_c)
			{ 
			}
			Subs(Channel* in_pChannel, C* in_obj, callback in_c) 
				: SubsImpl<T>(in_pChannel), m_obj(in_obj), m_c(in_c) 
			{ 
			}
			virtual void accept(void * in_data) 
			{ 
				value = *(T*)in_data; 
				(m_obj->*m_c)(value);
			}
	};
	
	template <class T> class Subs<T, UnusedTag>	: public SubsImpl<T> //{{{1
	{
		public:
			Subs() { /* not subscribed */ }
			Subs(Channel* in_parent, const char* in_name) : SubsImpl<T>(in_parent, in_name){ }
			Subs(Channel* in_pChannel) : SubsImpl<T>(in_pChannel) { }
			virtual void accept(void * in_data) { value = *(T*)in_data; }
	};
	
	class Dir : public SubsImpl<UnusedTag> //{{{1
	{
		public:
			Dir() { subscribe(0); }
			virtual void accept(void *) {}
	};
	//}}}

	SubsBase* processSubs();
	void waitFor(SubsBase& in_subs);
}

#endif // MSG_H_INCLUDED

