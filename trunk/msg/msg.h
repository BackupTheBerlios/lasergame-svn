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
		public:
			SubsBase(const char* in_name);
			virtual ~SubsBase();
			virtual TaskItemBase* createTaskItem(SubsBase* in_targetSubs) const = 0;
			void publish() const;
	};

	template <class T> class Subs : public SubsBase
	{
		public: // because of a bug in msvc-7.1
			T m_data;

		public:
			Subs(const char* in_name) : SubsBase(in_name) {}
			operator T& () { return m_data; }
			virtual TaskItemBase* createTaskItem(SubsBase* in_targetSubs) const
			{
				return new TaskItem<T>(in_targetSubs, m_data);
			}
	};
	
	template <class T> void TaskItem<T>::assign()
	{
		dynamic_cast<Subs<T>*>(m_subs)->m_data = m_data;
	}

	void wait();
}

#endif // MSG_H_INCLUDED

