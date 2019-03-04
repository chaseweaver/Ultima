#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "Queue.h"
#include "Scheduler.h"
#include <pthread.h>
#include <iostream>

class Semaphore {
private:
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	std::string resource_name;
	int sema_value;
	Queue<int> semaphore_list;
	Scheduler* scheduler;

public:

	/*
	* Semaphore::Semaphore(std::string, int, Scheduler*)
	* Default constructor.
	*/
	Semaphore(std::string name, int max_threads, Scheduler* s) :
		resource_name(name), sema_value(max_threads), scheduler(s) {}

	/*
	* Semaphore::Semaphore(std::string, int)
	* Default constructor.
	*/
	Semaphore(std::string name, int max_threads) :
		resource_name(name), sema_value(max_threads) {}

	/*
	* Semaphore::~Semaphore()
	* Default deconstructor.
	*/
	~Semaphore() {}

	/*
	* Semaphore::down(TASK_CONTROL_BLOCK*)
	* Locks multi-thread access to resources.
	*/
	void down(TASK_CONTROL_BLOCK* tcb) {
		if (sema_value > 0) {
			--sema_value;
		} else {
			scheduler->set_state(tcb, BLOCKED);
			semaphore_list.push(tcb->task_id);
			
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond, &mutex);
		}
	}

	/*
	* Semaphore::down(int)
	* Locks multi-thread access to resources.
	*/
	void down(int tid) {
		if (sema_value > 0) {
			--sema_value;
		} else {				
			semaphore_list.push(tid);
			pthread_mutex_lock(&mutex);
			pthread_cond_wait(&cond, &mutex);
		}
	}

	/*
	* Semaphore::up()
	* Releases lock to resources.
	*/
	void up() {
		if (semaphore_list.size() > 0) {
			semaphore_list.wait_and_pop();
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&cond);
		} else {
			++sema_value;
		}
	}
};

#endif