#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include <condition_variable>
#include <mutex>

class Semaphore {
private:
	std::mutex mutex;
	std::condition_variable cond;
	std::string resource_name;
	Queue<int> sema_queue;
	int value;
	int wakeups = 0;

public:
	Semaphore(std::string, int);
	~Semaphore();
	void wait();
	bool try_wait();
	void signal();
};

#endif