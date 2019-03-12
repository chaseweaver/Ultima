#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include <thread>             
#include <mutex>              
#include <condition_variable> 

class Semaphore {
private:
	std::mutex mtx;
	std::condition_variable cv;
	std::string resource_name;

	int value;

	Queue<TASK_CONTROL_BLOCK*> sema_queue;
	MASTER_CONTROL_BLOCK* master_control_block;

public:
	Semaphore(MASTER_CONTROL_BLOCK*, std::string, int);
	~Semaphore();
	void wait();
	void wait(TASK_CONTROL_BLOCK*);
	bool try_wait();
	void signal();
	std::string fetch_log();
	void set_state(TASK_CONTROL_BLOCK* tcb, int state);
};

#endif