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

namespace msg
{
	class TaskBase
	{
		public:  class Impl;
		protected: Impl* m_pimpl;

		public:
			// Creates Impl (thread-specific things = lock, item queue)
			TaskBase();
			// Destroys Impl
			virtual ~TaskBase();
	};
	
	class Task : public TaskBase
	{
		
	};

	class SubsBase;
	class WrappedBase
	{
		public: virtual void assignTo(SubsBase* in_subs) const = 0;
	};

	class SubsList;
	
	class SubsBase
	{
		SubsList& m_subList;
		TaskBase::Impl& m_taskImpl;
		public:
			SubsBase(const char* in_name);
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
			Subs(const char* in_name) : SubsBase(in_name) {}
			virtual WrappedBase* createWrapped() const
			{
				return new Wrapped<T>(*this);
			}
	};
	
	void wait();
}

#endif // MSG_H_INCLUDED

