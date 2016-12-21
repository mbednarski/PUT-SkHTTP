#include "Task.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void PrintFooTask::Run(void)
{
	cout << rand() << endl;
}

Task::~Task()
{
	
}
PrintFooTask::~PrintFooTask()
{
	
}
