/** 
 * @file
 * Implements threading interface from util/thread.h using Win32 API
 * 
 * @author Zbynek Winkler (c) 2004
 * $Id$
 */

#include "util/thread.h"
#include "util/assert.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace 
{
	struct Helper //{{{1
	{
		thread::proc_t m_proc;
		void* m_data;
		Helper(thread::proc_t in_proc, void* in_data) : m_proc(in_proc), m_data(in_data) {}
	};

	ulong __stdcall win32wrapper(void* in_help) //{{{1
	{
		Helper local(*(Helper*)in_help);
		delete (Helper*)in_help;
		return (local.m_proc)(local.m_data);
	}
	//}}}
}

namespace thread
{
	id_t create(proc_t in_proc, void* in_data) //{{{1
	{
		return (id_t)CreateThread(NULL, 0, win32wrapper, new Helper(in_proc, in_data), 0, 0);
	}
	
	ulong join(id_t in_id) //{{{1
	{
		ASSERT( sizeof(in_id) == sizeof(HANDLE) );
		DWORD ret UNUSED = WaitForSingleObject((HANDLE)in_id, INFINITE);
		ASSERT( ret != WAIT_FAILED );
		ulong retval;
		ret = GetExitCodeThread((HANDLE)in_id, &retval);
		CloseHandle((HANDLE)in_id);
		ASSERT( ret );
		return retval;
	}
	
	Key::Key() //{{{1
	{
		ASSERT( sizeof(key_type) == sizeof(DWORD) );
		m_key = (key_type)TlsAlloc();
	}
	
	Key::~Key() { TlsFree((DWORD)m_key); }
	void Key::operator = (void* in_data) { TlsSetValue((DWORD)m_key, in_data); }
	void* Key::getVoid() { return TlsGetValue((DWORD)m_key); }

	Lock::Lock() //{{{1
	{ 
		ASSERT( sizeof(HANDLE) == sizeof(lock_type) ); 
		m_lock = (lock_type)CreateMutex(0, FALSE, 0); 
	}

	Lock::~Lock()       { CloseHandle((HANDLE)m_lock); }
	void Lock::lock()   { WaitForSingleObject((HANDLE)m_lock, INFINITE); }
	void Lock::unlock() { ReleaseMutex((HANDLE)m_lock); }
	//}}}
}

