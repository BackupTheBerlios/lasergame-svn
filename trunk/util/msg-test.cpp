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
		Subs<bool> root(0);
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
		Task a(new TestRun(1));
	}

	class A : public Runnable //{{{1
	{
		public:
			Channel* m_quit;
			A(Channel* in_pChannel) : m_quit(in_pChannel) {}
			virtual void main() 
			{
				Subs<bool> quit(m_quit);
				quit.value = true; 
				quit.publish(); 
			}
	};
	
	AUTOTEST(channel) //{{{1
	{
		Subs<bool> quit(0);
		Task a(new A(quit));
	}

	class B : public Runnable //{{{1
	{
		Channel* m_a;
		Channel* m_b;
		public:
			B(Channel* in_a, Channel* in_b) : m_a(in_a), m_b(in_b) {}
			virtual void main()
			{
				Subs<bool> a(m_a), b(m_b);
				b.publish();
			}
	};

	AUTOTEST(waitFor) //{{{1
	{
		Subs<bool> a(0);
		Subs<bool> b(0);
		Task aa(new B(a, b));
		waitFor(b);
	}

	struct CallbackTest : public Runnable //{{{1
	{
		Channel* m_channel;
		Subs<bool, CallbackTest> m_a;
		bool m_beenHere;
		CallbackTest(Channel* in_channel) : m_channel(in_channel), m_a(this, &CallbackTest::handle) 
		{ 
			m_a.value = false;
		} 
		virtual void main()
		{
			m_a.subscribe(m_channel);
			m_a.publish();
			m_beenHere = false;
			//cout << "CallbackTest: waiting for m_a ..." << endl;
			waitFor(m_a);
			REQUIRE( m_beenHere == true );
			REQUIRE( m_a.value == true );
			m_a.publish();
		}
		void handle() { m_beenHere = true; }
	};

	AUTOTEST(callback) //{{{1
	{
		Subs<bool> a(0);
		Task cc(new CallbackTest(a));
		//cout << "callback: waiting for a ..." << endl;
		waitFor(a);
		a.value = true;
		a.publish();
		//cout << "callback: waiting for a again ..." << endl;
		waitFor(a);
	}
	//}}}
}

