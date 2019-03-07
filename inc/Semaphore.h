#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include "Scheduler.h"
#include <pthread.h>
#include <iostream>
#include <mutex>

class Semaphore {
private:
	Queue<int>* semaphore_list;
	std::string resource_name;
	int sema_value;

  mutable std::mutex m;
  std::condition_variable cond;
	MASTER_CONTROL_BLOCK* master_control_block;

public:
	Semaphore(std::string name, int max_threads)
		: resource_name(name), sema_value(max_threads) {}
	~Semaphore() {}
	void down(TASK_CONTROL_BLOCK*);
	void down(int tid);
	void up();
};

#endif