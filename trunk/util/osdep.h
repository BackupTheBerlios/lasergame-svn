#ifndef UTIL_OSDEP_H_INCLUDED
#define UTIL_OSDEP_H_INCLUDED 1

// GCC {{{1
#if defined __GNUC__   
#	define EXPORT // disable __declspec()
#	define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))
static inline unsigned int timestamp(void)
{
  unsigned long long t;
  rdtscll(t);
  return t >> 8;
}
#	if !defined __cdecl
#		define __cdecl __attribute__((__cdecl__))
#	endif

#define UNUSED __attribute__((unused))
#define NONRET __attribute__((noreturn))
#define CONST __attribute__((const))

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

// Microsoft Visual C++ {{{1
#elif defined _MSC_VER 
#	define inline __inline
#	pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
static inline unsigned int timestamp(void) { return 0; }
#	define snprintf _snprintf

#define UNUSED
#define NONRET
#define CONST

#define likely(x) (x)
#define unlikely(x) (x)

// ERROR {{{1
#else 
#	error Only GCC and MSVC supported and tested.
#endif //}}}

#define PTHREAD 1
#define WIN32THREAD 2

// Linux {{{1
#if defined linux

#include <sys/mman.h>
#	define O_BINARY 0
#	define THREAD PTHREAD

// Windows {{{1
#elif defined _WIN32

#define THREAD WIN32THREAD
#define bzero(a,b) memset(a, 0, b)
#define MAP_FAILED	(void*)-1 
#define mmap(a,b,c,d,e,f) MAP_FAILED 

// ERROR {{{1
#else
#	error Only Linux and Win32 supported and tested.
#endif

#endif // UTIL_OSDEP_H_INCLUDED

//#include <stdint.h>

//typedef unsigned char byte;
//typedef unsigned int uns;

//#ifndef linux

//#include <crtdbg.h>
//#define alloca _alloca
//#define inline _inline
//#define snprintf _snprintf
//#define usleep(x) Sleep(x)
//#define bcopy(a,b,c) memcpy(a,b,c)
//#define fileno(a) _fileno(a)
//#define setmode(a,b) _setmode(a,b)
//#define O_BINARY _O_BINARY


//#define NONRET
//#define UNUSED
//#define CONST
//#define PACKED
//#define likely(x) (x)
//#define unlikely(x) (x)

//#else

//#ifndef __KERNEL__

//#define setmode(a,b) do {} while (0)

//#define PACKED __attribute__((packed))

//#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
//#define __builtin_expect(x, expected_value) (x)
//#endif
//#define likely(x) __builtin_expect((x),1)
//#define unlikely(x) __builtin_expect((x),0)

//#ifdef __cplusplus
//template<class A> const A& min(const A& a, const A& b) { return a < b ? a : b; }
//template<class A> const A& max(const A& a, const A& b) { return a > b ? a : b; }
//#else
//#define MIN(a,b) ((a)<(b)?(a):(b))
//#define MAX(a,b) ((a)>(b)?(a):(b))
//#endif
//#define CLAMP(x,lo,hi) ((x)<(lo) ? (lo) : (x)>(hi) ? (hi) : x)

//#ifdef DEBUG
//void NONRET assert_failed(char *file, int line);
//#define ASSERT(x) do { if (!likely(x)) assert_failed(__FILE__, __LINE__); } while(0)
//#else
//#define ASSERT(x) do { } while(0)
//#endif

//#endif

//#endif


//static inline int sign(double x) { return (x > 0) - (x < 0); }

//void NONRET die(char *message, ...);
//void *xmalloc(unsigned int size);
//void dump_video(void);


