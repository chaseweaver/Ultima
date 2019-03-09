#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include <pthread.h>

class Semaphore {
private:
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	std::string resource_name;
	int value;

	Queue<TASK_CONTROL_BLOCK*> sema_queue;
	MASTER_CONTROL_BLOCK* master_control_block;

public:
	Semaphore(std::string, int);
	~Semaphore();
	void wait();
	void wait(TASK_CONTROL_BLOCK*);
	bool try_wait();
	void signal();
	std::string fetch_log();
};

#endif