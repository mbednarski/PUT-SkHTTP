#ifndef _TASK_H_
#define _TASK_H_

class Task
{
public:
	virtual void Run(void) = 0;
	virtual ~Task();
};

class PrintFooTask : public Task
{
public:
	virtual void Run(void);
	~PrintFooTask();
};

#endif
