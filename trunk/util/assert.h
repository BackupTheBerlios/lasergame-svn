#ifndef UTIL_ASSERT_H_INCLUDED
#define UTIL_ASSERT_H_INCLUDED 1

/**
 * @file
 *   A way to bridge asserts between MSVC and GNUC 
 * 
 * @author Zbynek Winkler (c) 2004
 * $Id$
 */

#ifdef _DEBUG
#	if defined _MSC_VER
#		include <crtdbg.h>
#		define ASSERT(expr) _ASSERTE(expr)

#	elif defined __GNUC__
#		include <cassert>
#		define ASSERT(expr) assert(expr)

#	else
#		error Only MSVC and GNUC supported and tested.
#	endif

#elif defined(NDEBUG) //DEBUG

#	define ASSERT(haha)	do {} while(0)

#else // !(_DEBUG && NDEBUG)
#	error "Neither _DEBUG nor NDEBUG defined."
#endif

#endif // UTIL_ASSERT_H_INCLUDED

