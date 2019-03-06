#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "Queue.h"
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

struct ARGUMENTS {
	int id;
	int thread_results;
	struct TASK_CONTROL_BLOCK* task_control_block;
};

struct TASK_CONTROL_BLOCK {
	int task_id;
	int task_state;
	std::string task_name;
	pthread_t task_thread;
	ARGUMENTS* task_arguments;
};

class Scheduler {
private:

	Queue<TASK_CONTROL_BLOCK*> task_list;
	int number_of_workers = 0;

	void yield(int, int);
	void scheduler();

public:
	Scheduler();
	~Scheduler();
	void create_new_task(std::string, void*(void*), ARGUMENTS*);
	void set_state(TASK_CONTROL_BLOCK*, int);
	static void* start_scheduler(void*);
	ARGUMENTS* create_arguments(int, int);
	ARGUMENTS* create_arguments(int, int, TASK_CONTROL_BLOCK*);
};

#endif