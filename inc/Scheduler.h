#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "ThreadSafeQueue.h"
#include "Struct.h"
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

#define STATE_WINDOW -3

#define DEAD 0
#define BLOCKED 1
#define IDLE 2
#define READY 3
#define RUNNING 4

class Scheduler {
private:

	MASTER_CONTROL_BLOCK* master_control_block;
	ThreadSafeQueue<TASK_CONTROL_BLOCK*> task_list;
	int number_of_workers = 0;

	void yield(int, int);
	void scheduler();

public:
	Scheduler(MASTER_CONTROL_BLOCK* mcb);
	~Scheduler();
	void create_new_task(std::string, void*(void*), ARGUMENTS*);
	void set_state(TASK_CONTROL_BLOCK*, int);
	int task_list_size();
	static void* start_scheduler(void*);
	ARGUMENTS* create_arguments(int, int);
	ARGUMENTS* create_arguments(int, int, TASK_CONTROL_BLOCK*);
};

#endif