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
	
	class FactoryBase 
	{ 
		public: 
			TaskImpl* m_taskImpl; 
			virtual Runnable* create() = 0; 
			virtual ~FactoryBase() {} 
	};

	// class factories templates {{{1
	struct UnusedTag { void func() {} };
	
	template <class What, class P1=UnusedTag, class P2=UnusedTag, class P3=UnusedTag, class P4=UnusedTag> //{{{2
		class Factory : public FactoryBase
	{
		P1 m_par1; P2 m_par2; P3 m_par3; P4 m_par4;
		public:
			Factory(const P1 in_p1, const P2 in_p2) : m_par1(in_p1), m_par2(in_p2), m_par3(in_p3), m_par4(in_p4) {}
			virtual Runnable* create() { return new What(m_par1, m_par2); }
	};

	template <class What, class P1, class P2, class P3> //{{{2
		class Factory<What, P1, P2, P3, UnusedTag> : public FactoryBase
	{
		P1 m_1; P2 m_2; P3 m_3;
		public:
			Factory(const P1 in_1, const P2 in_2, const P3 in_3) : m_1(in_1), m_2(in_2), m_3(in_3) {}
			virtual Runnable* create() { return new What(m_1, m_2, m_3); }
	};

	template <class What, class P1, class P2> //{{{2
		class Factory<What, P1, P2, UnusedTag, UnusedTag> : public FactoryBase
	{
		P1 m_1; P2 m_2;
		public:
			Factory(const P1 in_1, const P2 in_2) : m_1(in_1), m_2(in_2) {}
			virtual Runnable* create() { return new What(m_1, m_2); }
	};

	template <class What, class P1> //{{{2
		class Factory<What, P1, UnusedTag, UnusedTag, UnusedTag> : public FactoryBase
	{
		P1 m_1;
		public:
			Factory(const P1 in_1) : m_1(in_1) {}
			virtual Runnable* create() { return new What(m_1); }
	};

	template <class What> //{{{2
		class Factory<What, UnusedTag, UnusedTag, UnusedTag, UnusedTag> : public FactoryBase
	{
		public:
			virtual Runnable* create() { return new What(); }
	};
	//}}}1
	
	// factories making helpers {{{1
	template <class What> //{{{2
		FactoryBase* factory()
	{
		return new Factory<What>();
	}
	
	template <class What, class P1>  //{{{2
		FactoryBase* factory(const P1 in_p1)
	{
		return new Factory<What, P1>(in_p1);
	}

	template <class What, class P1, class P2>  //{{{2
		FactoryBase* factory(const P1 in_p1, const P2 in_p2)
	{
		return new Factory<What, P1, P2>(in_p1, in_p2);
	}
	
	template <class What, class P1, class P2, class P3>  //{{{2
		FactoryBase* factory(const P1 in_p1, const P2 in_p2, const P3 in_p3)
	{
		return new Factory<What, P1, P2, P3>(in_p1, in_p2, in_p3);
	}

	template <class What, class P1, class P2, class P3, class P4>  //{{{2
		FactoryBase* factory(const P1 in_p1, const P2 in_p2, const P3 in_p3, const P4 in_p4)
	{
		return new Factory<What, P1, P2, P3, P4>(in_p1, in_p2, in_p3, in_p4);
	}
	//}}}1

	class Task //{{{1
	{
		TaskImpl* m_pimpl;
		public:
			/// creates thread
			Task(FactoryBase* in_factory);
			/// waits for its thread to finish
			~Task();
	}; //}}}

	class Channel;
	class bad_type {};
	class SubsBase //{{{1
	{
		Channel* m_pChannel;
		TaskImpl& m_taskImpl;
		protected:
			void subscribe();
			void subscribe(Channel* in_parent, const char* in_name);
			void subscribe(Channel* in_pChannel);
			void unsubscribe();
		public:
			SubsBase();
			Channel* getChannel() { return m_pChannel; }
			virtual ~SubsBase() { }
			virtual void* create() const = 0;
			virtual void destroy(void* in_data) const = 0;
			virtual void checkType(SubsBase*) const = 0;
			virtual void execute(void*) = 0;
			void publish() const;
			operator Channel* () { return m_pChannel; }
			void postItem(void* data);
	};//}}}1

	namespace detail {
		/// Go through the task queue and delete all messages destined to the supplied subs
		void destroyItemsFor(SubsBase* in_subs);
	}

	template <class T> class SubsImpl : public SubsBase
	{
		private:
			SubsImpl(const SubsImpl&) {}
		public:
			T value;
			SubsImpl() { subscribe(); }
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

	template <class T, class C = UnusedTag> 
		class Subs : public SubsImpl<T>
	{
		public:
			typedef void (C::*callback)();
			C* m_obj;
			const callback m_c;
		public:
			Subs(C* in_obj, callback in_c) : m_obj(in_obj), m_c(in_c) { }
			Subs(Channel* in_parent, const char* in_name, C* in_obj, callback in_c) 
				: SubsImpl<T>(in_parent, in_name), m_obj(in_obj), m_c(in_c)
			{ 
			}
			Subs(Channel* in_pChannel, C* in_obj, callback in_c) 
				: SubsImpl<T>(in_pChannel), m_obj(in_obj), m_c(in_c) 
			{ 
			}
			virtual void execute(void * in_data) 
			{ 
				value = *(T*)in_data; 
				(m_obj->*m_c)();
			}
	};
	
	template <class T> class Subs<T, UnusedTag>	: public SubsImpl<T>
	{
		public:
			Subs() { }
			Subs(Channel* in_parent, const char* in_name) : SubsImpl<T>(in_parent, in_name){ }
			Subs(Channel* in_pChannel) : SubsImpl<T>(in_pChannel) { }
			virtual void execute(void * in_data) { value = *(T*)in_data; }
	};

	SubsBase* processSubs();
	void waitFor(SubsBase& in_subs);
}

#endif // MSG_H_INCLUDED

