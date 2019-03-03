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

	/*
	 * Scheduler::yield(int, int)
	 * Alternative to pthread_yield() since that is not cross-platform.
	 */
	void yield(int min, int max) {
		sleep(min + rand() % max);
	}

	void scheduler() {
		srand(time(NULL));
		do {
			if (!task_list.empty()) {
				TASK_CONTROL_BLOCK* tcb = task_list.dequeue();
				task_list.enqueue(tcb);
				yield(1, 2);

				switch(tcb->task_state) {
					case DEAD:

					case BLOCKED:
						set_state(tcb, READY);
						break;

					case READY:
						set_state(tcb, RUNNING);
						break;

					case RUNNING:
						set_state(tcb, READY);
						break;
				}
			}
		} while (true);
	}

public:
	Scheduler() {
		// Spawns scheduler loop in a new thread
		// assert(!pthread_create(new pthread_t, NULL, start_scheduler, this));
	}

	~Scheduler() {}

	/*
	 * Scheduler::create_new_task(std::string, void* (void*), ARGUMENTS*)
	 * Creates a new TASK_CONTROL_BLOCK and spawns a new child function in a new thread.
	 */
	void create_new_task(std::string task_name, void* worker_function(void* arguments), ARGUMENTS* task_arguments) {

		TASK_CONTROL_BLOCK* tcb = new TASK_CONTROL_BLOCK;
		tcb->task_id = ++number_of_workers;
		tcb->task_state = READY;
		tcb->task_name = task_name;
		tcb->task_thread = *(new pthread_t);
		tcb->task_arguments = task_arguments;

		task_list.enqueue(tcb);
		assert(!pthread_create(&tcb->task_thread, NULL, worker_function, &tcb->task_arguments));
	}

	/*
	 * Scheduler::set_state(TASK_CONTROL_BLOCK*, int)
	 * Changes task state in the list.
	 */
	void set_state(TASK_CONTROL_BLOCK* tcb, int state) {
		tcb->task_state = state;
	}

	/*
	 * Scheduler::start_scheduler(void*)
	 * Starts the scheduler loop in a new thread.
	 */
	static void* start_scheduler(void* p) {
		static_cast<Scheduler*>(p)->scheduler();
		return NULL;
	}

	/*
	 * Scheduler::create_arguments(int, int)
	 * Creates a new ARGUMENTS struct.
	 */
	ARGUMENTS create_arguments(int id, int thread_results) {
		ARGUMENTS args;
		args.id = id;
		args.thread_results = thread_results;
		return args;
	}
};

#endif