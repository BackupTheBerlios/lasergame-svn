#include "conf.h"
#include <sstream>
#include <iostream>

namespace conf {

	Robot::Robot(const char* in_string)
	{
		using namespace std;
		string in(in_string);
		string::size_type start = 0;
		do
		{
			string::size_type end = in.find_first_of(",| ", start);
			//cout << '[' << start << ", " << end << ']' << '"' << in.substr(start, end-start) << '"' << endl;
			push_back(in.substr(start, end-start));
			start = in.find_first_not_of(",| ", end);
		} while (start != in.npos);
		
#if 0
		istringstream in(in_string);
		string s;
		while(getline(in >> ws, s, ','))
			push_back(s);
#endif
	}
}

