#include "util/unit-test.h"
#include "msg/msg.h"
#include "msg/task.h"

namespace
{
	class Int : public msg::Message
	{
		int m_int;
		public:
			Int() : m_int(0) {}
			bool operator == (const int & in_int) { return m_int == in_int; }
			void set(const int & in_int) { m_int = in_int; }
	};

	AUTOTEST(test1)
	{
		msg::MessageImpl<Int> sender("number");
		msg::MessageImpl<Int> receiver("number");
		msg::MessageImpl<Int> nonReceiver("number1");

		REQUIRE( receiver == 0 );
		sender.set(5);
		sender.publish();
		task::wait();
		REQUIRE( receiver == 5 );
	}
}

