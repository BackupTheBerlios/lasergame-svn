/** 
 * @file
 * Implements threading interface from util/thread.h using pthread API
 * 
 * @author Zbynek Winkler (c) 2004
 * $Id$
 */

#include "util/thread.h"
#include "util/assert.h"

#include <ptread.h>

namespace thread
{
	id_t create(proc_t in_proc, void* in_data) //{{{1
	{
		ASSERT( sizeof(id_t) == sizeof(pthread_id) );
		//return (id_t)CreateThread(NULL, 0, win32wrapper, new Helper(in_proc, in_data), 0, 0);
		pthread_id id;
		pthread_create(&id, NULL, in_proc, in_data);
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
		pthread_key_create(&m_key, NULL);
	}
	
	Key::~Key() {  pthread_key_delete(m_key);  }
	void Key::operator = (void* in_data) { pthread_setspecific(m_key, in_data); }
	void* Key::getVoid() { return pthread_getspecific(in_key); }

	Lock::Lock() //{{{1
	{ 
		ASSERT( sizeof(pthread_mutex_t) == sizeof(lock_type) ); 
		pthread_mutex_init(&m_lock, NULL);
	}

	Lock::~Lock()       { pthread_mutex_destroy(&m_lock); }
	void Lock::lock()   { pthread_mutex_lock(&m_lock); }
	void Lock::unlock() { pthread_mutex_unlock(&m_lock); }
	//}}}
}

