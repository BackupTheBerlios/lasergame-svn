#include "task.h"

namespace task
{
	Task s_main;

	Task::Task()
	{
	}

	Task::~Task()
	{
	}

	void wait()
	{
	}

	Task& current()
	{
		return s_main;
	}
}

