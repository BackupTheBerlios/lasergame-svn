#ifndef UTIL_CONF_H_INCLUDED
#define UTIL_CONF_H_INCLUDED 1

#include <vector>
#include <string>

namespace conf {

	class Robot : public std::vector<std::string>
	{
		public:
			Robot() {}
			Robot(const char* in);
	};
	
}

#endif // UTIL_CONF_H_INCLUDED

