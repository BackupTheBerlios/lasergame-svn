#include <iostream>
#include "util/unit-test.h"
#include "util/osdep.h"
#include "msg/msg.h"
using namespace std;

namespace
{
	using namespace msg;

	class Int //{{{1
	{
		int m_int;
		public:
		Int(const Int& in_int) { m_int = in_int.m_int; }
			Int() : m_int(0) {}
			bool operator == (const int & in_int) { return m_int == in_int; }
			void set(const int & in_int) { m_int = in_int; }
	};
	
	class TestRun : public Runnable
	{
		private:
			TestRun(const TestRun&) {}
			int m_int;
		public:
			TestRun(int in_int) : m_int(in_int) {}
			virtual void main()
			{
				//cout << "I am here! " << m_int << endl;
			}
		};

	AUTOTEST(sendReceiveSingle) //{{{1
	{
		Subs<Int> sender("number");
		Subs<Int> receiver("number");
		Subs<Int> nonReceiver("aaa");

		REQUIRE( receiver == 0 );
		sender.set(5);
		sender.publish();
		wait();
		REQUIRE( receiver == 5 );
	} 

	int goDist(int )
	{
		return 0;
	};
	
	AUTOTEST(subTask) //{{{1
	{
		//Channel<Int> q;
		//NamedChannel<Int> nq("quit");
		//Subs<Int> s(q);
		//Task t(helper(&goDist, 1));
		// Task t(helper(&monitor, "quit"));
		// Subs<Int> q("quit");
		// wait(q);
		Task a(new Helper<TestRun, int>(1));
		Task b(helper<TestRun>(2));
		TASK(TestRun, 3);
	}
	//}}}
}

