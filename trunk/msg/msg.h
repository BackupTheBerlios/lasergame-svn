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
	
	void wait();
}

#endif // MSG_H_INCLUDED

