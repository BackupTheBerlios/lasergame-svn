/** 
 * @file
 * Implements threading interface from util/thread.h using pthread API
 * 
 * @author Zbynek Winkler (c) 2004
 * $Id$
 */

#include "util/thread.h"
#include "util/assert.h"

#include <pthread.h>

namespace thread
{
	id_t create(proc_t in_proc, void* in_data) //{{{1
	{
		ASSERT( sizeof(id_t) == sizeof(pthread_t) );
		//return (id_t)CreateThread(NULL, 0, win32wrapper, new Helper(in_proc, in_data), 0, 0);
		pthread_t id;
		pthread_create(&id, NULL, (void* (*) (void*)) in_proc, in_data);
		// check return?
		
		return (id_t)id;
	}
	
	ulong join(id_t in_id) //{{{1
	{
		void *retval;
		pthread_join(in_id, &retval);
		return (ulong)retval;
	}
	
	Key::Key() //{{{1
	{
		ASSERT( sizeof(key_type) == sizeof(pthread_key_t) );
		pthread_key_create((pthread_key_t*)&m_key, NULL);
	}
	
	Key::~Key() {  pthread_key_delete(m_key);  }
	void Key::operator = (void* in_data) { pthread_setspecific(m_key, in_data); }
	void* Key::getVoid() { return pthread_getspecific(m_key); }

	Lock::Lock() //{{{1
	{ 	
		ASSERT( sizeof(pthread_mutex_t*) == sizeof(lock_type) ); 
		m_lock = new pthread_mutex_t;
		pthread_mutex_init((pthread_mutex_t*)m_lock, NULL);
	}

	Lock::~Lock()       { pthread_mutex_destroy((pthread_mutex_t*)m_lock); delete (pthread_mutex_t*)m_lock; }
	void Lock::lock()   { pthread_mutex_lock((pthread_mutex_t*)m_lock); }
	void Lock::unlock() { pthread_mutex_unlock((pthread_mutex_t*)m_lock); }

	CondVar::CondVar()
	{
	  m_cv = new pthread_cond_t;
	  pthread_cond_init((pthread_cond_t*)m_cv, NULL);
	}
	CondVar::~CondVar()
	{
	  pthread_cond_destroy((pthread_cond_t*)m_cv);
	}
	void CondVar::wait(Lock& in_l)
	{
	  pthread_cond_wait((pthread_cond_t*)m_cv,(pthread_mutex_t*)in_l.m_lock);
	}
	void CondVar::broadcast()
	{
	  pthread_cond_broadcast((pthread_cond_t*)m_cv);
	}
	//}}}
}

