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

	class TaskItemBase
	{
		protected:
			SubsBase* m_subs;
		
		public:
			TaskItemBase(SubsBase* in_targetSubs) : m_subs(in_targetSubs) {}
			virtual void assign() = 0;
			virtual ~TaskItemBase() {}
			bool isFor(SubsBase* in_subs) { return m_subs == in_subs; }
	};

	template <class T> class TaskItem : public TaskItemBase
	{
		T m_data;
		
		public:
			TaskItem(SubsBase* in_targetSubs, const T & in_data) 
				: TaskItemBase(in_targetSubs), m_data(in_data)
			{
			}
			virtual void assign(); // defined later in this file
	};

	class SubsList;
	
	class SubsBase
	{
		SubsList& m_subList;
		TaskBase::Impl& m_taskImpl;
		public:
			SubsBase(const char* in_name);
			virtual ~SubsBase();
			virtual TaskItemBase* createTaskItem(SubsBase* in_targetSubs) const = 0;
			void publish() const;
	};

	template <class T> class Subs : public SubsBase, public T
	{
		public:
			Subs(const char* in_name) : SubsBase(in_name) {}
			virtual TaskItemBase* createTaskItem(SubsBase* in_targetSubs) const
			{
				return new TaskItem<T>(in_targetSubs, *this);
			}
	};
	
	template <class T> void TaskItem<T>::assign()
	{
		*dynamic_cast<T*>(m_subs) = m_data;
	}

	void wait();
}

#endif // MSG_H_INCLUDED

