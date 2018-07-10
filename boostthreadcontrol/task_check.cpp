#include "task_check.h"
#include <iostream>
TaskCheck::TaskCheck()
{
}

bool TaskCheck::operator() ()
{
	

	std::cout << "taskcheck test" << std::endl;
	return true;
}


