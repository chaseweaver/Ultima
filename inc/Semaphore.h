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
	int value;
	int wakeups = 0;

public:
	Semaphore(int val);
	void wait();
	bool trywait();
	void signal();
};

#endif