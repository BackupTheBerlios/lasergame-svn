#ifndef UTIL_UNIT_TEST_H_INCLUDED
#define UTIL_UNIT_TEST_H_INCLUDED 1

/**
 * @file 
 *     Simplified unit testing framework
 *          
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id$
 */

#include <cstdlib>
#include <cstdio>

#define STRLINE STRINGIZE(__LINE__)

#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X

#define JOIN( X, Y ) DO_JOIN( X, Y )
#define DO_JOIN( X, Y ) DO_JOIN2(X,Y)
#define DO_JOIN2( X, Y ) X##Y

#define AUTOTEST(a) static void a(); Registrator JOIN(s,__LINE__)(a); static void a()

#define CHECK(pred)   do { if (!(pred)){puts("\n"__FILE__"("STRLINE") : "#pred); s_failed++;} } while(0)
#define REQUIRE(pred) do { if (!(pred)){puts("\n"__FILE__"("STRLINE") : "#pred); s_failed++;} } while(0)
#define ERROR(message) puts("\n" __FILE__ "(" STRLINE ") : " message)
#define FAIL(message)  puts("\n" __FILE__ "(" STRLINE ") : " message)

#define CPPUNIT_ASSERT(pred) REQUIRE(pred)
#define CPPUNIT_ASSERT_EQUAL(a,b) REQUIRE((a) == (b))
#define CPPUNIT_ASSERT_DOUBLES_EQUAL(a,b,c) REQUIRE((((a)-(b)) < (c)) && (((a)-(b)) > (-c)))

namespace 
{
	enum { MAX_TESTS = 20 };
	void (*s_tests[MAX_TESTS])();
	int s_numtests = 0;
	int s_failed = 0;
	struct Registrator
	{
		Registrator(void (*in_fn)())
		{
			if (s_numtests >= MAX_TESTS)
			{
				fputs(__FILE__ "(" STRLINE ") : Too many tests defined.", stdout);
				abort();
			}
			s_tests[s_numtests] = in_fn;
			s_numtests++;
		}
	};
}

int main(int argc, char* argv[])
{
	for (int i = 0; i < s_numtests; i++)
	{
		fputc('.', stdout);
		s_tests[i]();
	}
	if (s_failed)
		fprintf(stdout, "\nFAILED (%d) out of %d tests.\n", s_failed, s_numtests);
	else
		fprintf(stdout, "\nOK (%d)\n", s_numtests);
	return s_failed;
}

#endif // UTIL_UNIT_TEST_H_INCLUDED

