#include <iostream>
#include "util/unit-test.h"
#include "util/msg.h"
using namespace std;

namespace
{
	using namespace msg;

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
		Subs<bool> root;
		Subs<int> sender(root, "number");
		Subs<int> receiver(root, "number");
		Subs<int> nonReceiver(root, "aaa");

		receiver.value = 0;
		sender.value = 5;
		sender.publish();
		processSubs();
		REQUIRE( receiver.value == 5 );
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
	}

	class A : public Runnable //{{{1
	{
		public:
			Subs<bool> m_quit;
			A(Channel* in_pChannel) : m_quit(in_pChannel) {}
			static FactoryBase* fac(Channel* in_pChannel) { return factory<A>(in_pChannel); }
			virtual void main() 
			{ 
				m_quit.value = true; 
				m_quit.publish(); 
			}
	};
	
	AUTOTEST(channel) //{{{1
	{
		Subs<bool> quit;
		Task a(A::fac(quit));
	}

	class B : public Runnable //{{{1
	{
		Subs<bool> m_a, m_b;
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
		Subs<bool> a;
		Subs<bool> b;
		Task aa(new Factory<B, Channel*, Channel*>(a, b));
		Task bb(factory<B>(a.getChannel(), b.getChannel()));
		Task cc(B::fac(a, b));
		waitFor(b);
	}
	//}}}
}

