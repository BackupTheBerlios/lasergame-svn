#include <iostream>
#include "util/unit-test.h"
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
	
	class TestRun : public Runnable //{{{1
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

	int goDist(int ) //{{{1
	{
		return 0;
	};
	
	AUTOTEST(subTask) //{{{1
	{
		//Task t(Factory(&goDist, 1));
		// wait(q);
		Task a(new Factory<TestRun, int>(1));
		Task b(factory<TestRun>(2));
		TASK(TestRun, (3));
	}
	
	struct Bool { bool m_bool; }; //{{{1

	class A : public Runnable //{{{1
	{
		public:
			Subs<Bool> m_quit;
			A(Channel* in_pChannel) : m_quit(in_pChannel) {}
			virtual void main() 
			{ 
				m_quit.m_bool = true; 
				m_quit.publish(); 
			}
	};
	
	AUTOTEST(channel) //{{{1
	{
		Subs<Bool> quit;
		TASK(A, (quit.getChannel()));
	}

	class B : public Runnable //{{{1
	{
		Subs<Bool> m_a, m_b;
		public:
			B(Channel* in_a, Channel* in_b) : m_a(in_a), m_b(in_b) {}
			virtual void main()
			{
				m_b.publish();
			}
		//static FactoryBase* fac(Channel* in_a, Channel* in_b) { return new Factory<B,Channel*,Channel*>(in_a, in_b); }
			static FactoryBase* fac(Channel* in_a, Channel* in_b) { return factory<B>(in_a, in_b); }
	};

	AUTOTEST(waitFor) //{{{1
	{
		Subs<Bool> a;
		Subs<Bool> b;
		Task aa(new Factory<B, Channel*, Channel*>(a, b));
		Task bb(factory<B>(a.getChannel(), b.getChannel()));
		Task cc(B::fac(a, b));
		wait(b);
	}
	//}}}
}

