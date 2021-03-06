#ifndef UTIL_THREAD_H_INCLUDED
#define UTIL_THREAD_H_INCLUDED 1

/**
 * @file
 *   Threading interface to be implemented by Win32 and PThread APIs
 * 
 * @author Zbynek Winkler (c) 2004
 *
 * $Id$
 */

#include "util/osdep.h"

typedef unsigned long ulong;

namespace thread
{
	typedef ulong id_t;
	typedef ulong (*proc_t)(void *);

	id_t create(proc_t, void*);
	ulong join(id_t);
	
	class Key
	{
		private:
			typedef ulong key_type;
			key_type m_key;
		public:
			Key();
			~Key();
			void operator = (void*);
			void* getVoid();
	};

	struct CondVar;

	struct Lock
	{
		private:
			typedef void* lock_type;
			lock_type m_lock;
		public:
			Lock();
			~Lock();
			void lock();
			void unlock();
		friend struct CondVar;
	};

	struct Locker
	{
		private:
			Lock& m_lock;
		public:
			Locker(Lock & in_lock) : m_lock(in_lock) { m_lock.lock(); }
			~Locker() { m_lock.unlock(); }
	};

	struct CondVar
	{
		private:
			typedef void* CondVar_type;
			CondVar_type m_cv;
		public:
			CondVar();
			~CondVar();
			void wait(Lock&);
			void broadcast();
	};
}

#endif // UTIL_THREAD_H_INCLUDED

