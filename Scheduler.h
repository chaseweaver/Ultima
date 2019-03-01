#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "LinkedList.h"
#include "Worker.h"
#include <pthread.h>
#include <iostream>
#include <assert.h>

#define DEAD 0
#define BLOCKED 1
#define IDLE 2
#define READY 3
#define RUNNING 4

struct TASK_CONTROL_BLOCK {
	int task_id;
	int task_state;
	std::string task_name;
	pthread_t task_thread;
	struct Worker::ARGUMENTS* task_arguments;
};

class Scheduler {
private:
	Worker* worker;
	LinkedList<TASK_CONTROL_BLOCK*> task_list;
	int number_of_workers = 0;

public:
	Scheduler(Worker* w) : worker(w) {}

	void create_new_task(std::string task_name, void* worker_function(void* arguments), Worker::ARGUMENTS* task_arguments) {

		TASK_CONTROL_BLOCK* tcb = new TASK_CONTROL_BLOCK;
		tcb->task_id = ++number_of_workers;
		tcb->task_state = READY;
		tcb->task_name = task_name;
		tcb->task_thread = *(new pthread_t);
		tcb->task_arguments = task_arguments;

		assert(!pthread_create(&tcb->task_thread, NULL, worker_function, &tcb->task_arguments));
	}

	/*
	 * Scheduler::set_state(TASK_CONTROL_BLOCK*, int)
	 * Changes task state in the list.
	 */
	void set_state(TASK_CONTROL_BLOCK* tcb, int state) {
		tcb->task_state = state;
	}
};

#endif