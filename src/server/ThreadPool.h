#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "Task.h"
#include <pthread.h>
#include <queue>
#include <vector>
#include "Logger.h"

void *ThreadFunc(void *);

class ThreadPool
{
private:
    // ilosc watkow w puli
	const size_t POOL_SIZE;
    // synchronizacja watkow
	pthread_mutex_t mutex;
	pthread_cond_t canDequeue;
    // kolejka z zadaniami do wykonania
	std::queue<Task *> taskQueue;
    // lista watkow pracujacych
	std::vector<pthread_t> threads;
    Logger log;
public:
	friend void *ThreadFunc(void *); //hate this but it's the easiest solution
	ThreadPool(size_t poolSize);
    // zakolejkuj zadanie
	void EnqueueTask(Task *);
    // rozpocznij wykonywanie zadan
	void Start(void);	
};

class ThreadArgs
{
public:
	int ThreadId;
	ThreadPool *ParentPool;
};

#endif
