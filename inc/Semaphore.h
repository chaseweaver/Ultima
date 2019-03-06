#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "ThreadSafeQueue.h"
#include "Scheduler.h"
#include <pthread.h>
#include <iostream>

class Semaphore {
private:
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	std::string resource_name;
	int sema_value;
	ThreadSafeQueue<int> semaphore_list;
	Scheduler* scheduler;

public:
	Semaphore(std::string name, int max_threads, Scheduler* s)
		: resource_name(name), sema_value(max_threads), scheduler(s) {}
	Semaphore(std::string name, int max_threads)
		: resource_name(name), sema_value(max_threads) {}
	~Semaphore() {}
	void down(TASK_CONTROL_BLOCK*);
	void down(int tid);
	void up();
};

#endif