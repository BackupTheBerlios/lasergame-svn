/**
 * @file 
 *     Tests threading API as declared in util/thread.h
 *          
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id$
 */

#include "util/unit-test.h"
#include "util/thread.h"

namespace
{
	//{{{1 CreateAndJoin
	ulong threadFn1(void* in_data) //{{{2
	{
		*(int*)in_data = 0xbabe;
		return 0;
	}
	
	AUTOTEST(CreateAndJoin) //{{{2
	{
		int var = 0;
		thread::id_t t = thread::create(threadFn1, &var);
		thread::join(t);
		REQUIRE( var == 0xbabe );
	} //}}}
	
	//{{{1 ThreadData
	ulong threadFn2(void* in_data) //{{{2
	{
		*(thread::Key*)in_data = (void*)0xbabe;
		REQUIRE( ((thread::Key*)in_data)->getVoid() == (void*)0xbabe );
		return 0;
	}
	
	AUTOTEST(ThreadData) //{{{2
	{
		thread::Key k;
		thread::id_t t = thread::create(threadFn2, &k);
		k = (void*)0xbeef;
		thread::join(t);
		REQUIRE( k.getVoid() == (void*)0xbeef );
	} //}}}
	
	//{{{1 Lock
	int sharedVar;
	ulong threadFn3(void* in_data) //{{{2
	{
		thread::Lock& l = *(thread::Lock*)in_data;
		l.lock();
		REQUIRE( sharedVar == 1 );
		sharedVar = 2;
		l.unlock();
		return 0;
	}
	
	AUTOTEST(Lock) //{{{2
	{
		thread::Lock l;
		l.lock();
		sharedVar = 1;
		thread::id_t t = thread::create(threadFn3, &l);
		while (sharedVar == 1)
		{
			l.unlock();
			l.lock();
		}
		REQUIRE( sharedVar == 2 );
		l.unlock();
		thread::join(t);
	} //}}}
	//}}}
}
