//#include <iostream>
#include "util/unit-test.h"
#include "msg/msg.h"
using namespace std;

namespace
{
	class Int
	{
		int m_int;
		public:
			Int() : m_int(0) {}
			bool operator == (const int & in_int) { return m_int == in_int; }
			void set(const int & in_int) { m_int = in_int; }
	};

	AUTOTEST(test1)
	{
		msg::Subs<Int> sender("number");
		msg::Subs<Int> receiver("number");
		msg::Subs<Int> nonReceiver("aaa");

		REQUIRE( Int(receiver) == 0 );
		Int(sender).set(5);
		sender.publish();
		msg::wait();
		REQUIRE( Int(receiver) == 5 );
	}
}

