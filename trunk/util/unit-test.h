#ifndef UTIL_UNIT_TEST_H_INCLUDED
#define UTIL_UNIT_TEST_H_INCLUDED 1

/**
 * @file 
 *     Simplified unit testing framework
 *          
 * @author Zbynek Winkler (c) 2004
 * 
 * $Id: $
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
	char buffer[100];
	if (s_failed)
	{
		fputs("\nFAILED (", stdout);
		itoa(s_failed, buffer, 10);
		fputs(buffer, stdout);
		fputs(") out of ", stdout);
		itoa(s_numtests, buffer, 10);
		fputs(buffer, stdout);
		fputs(" tests.\n", stdout);
	}
	else
	{
		fputs("\nOK (", stdout);
		itoa(s_numtests, buffer, 10);
		fputs(buffer, stdout);
		fputs(")\n", stdout);
	}
	return s_failed;
}

#endif // UTIL_UNIT_TEST_H_INCLUDED

