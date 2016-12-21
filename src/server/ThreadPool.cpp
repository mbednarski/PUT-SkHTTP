#include "ThreadPool.h"
#include <pthread.h>
#include <exception>
#include <iostream>
#include <cstdlib>
#include "Logger.h"
#include "util.h"

using namespace std;

ThreadPool::ThreadPool(size_t poolSize) : POOL_SIZE(poolSize),
	mutex(PTHREAD_MUTEX_INITIALIZER),	
	canDequeue(PTHREAD_COND_INITIALIZER)
{
	threads = vector<pthread_t>(POOL_SIZE);
}

void ThreadPool::EnqueueTask(Task *task)
{
	pthread_mutex_lock(&mutex);
	taskQueue.push(task);
	pthread_cond_signal(&canDequeue);
	pthread_mutex_unlock(&mutex);	
}

void ThreadPool::Start(void)
{
	for(size_t i = 0; i < POOL_SIZE; i++)
	{
		ThreadArgs *args = new ThreadArgs();
		args->ThreadId = i;
		args->ParentPool = this;
		
		int status = pthread_create(&threads[i], NULL, ThreadFunc, (void *) args);
		if(status)
		{
			cerr << "Failed to create thread. Return code: " << status;
			exit(1);
		}
		pthread_detach(threads[i]);
	}
}

void *ThreadFunc(void *args)
{
	ThreadArgs *threadArgs = (ThreadArgs *) args;
	queue<Task *> &taskQueue = threadArgs->ParentPool->taskQueue;
	// cout << "Created thread " << threadArgs->ThreadId << endl;
    Logger log;
    log.log("Created thread " + ToString(threadArgs->ThreadId));
	
	while(1)
	{
		pthread_mutex_lock(&threadArgs->ParentPool->mutex);
		while(taskQueue.empty())
		{
			pthread_cond_wait(&threadArgs->ParentPool->canDequeue, 
				&threadArgs->ParentPool->mutex);
		}
		Task *t = taskQueue.front();
		taskQueue.pop();				
		pthread_mutex_unlock(&threadArgs->ParentPool->mutex);		
		t->Run();	
		delete t;
	}
		
	return NULL;
}
