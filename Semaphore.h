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
		pthread_mutex_t mutex;
		pthread_cond_t cond;
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
	   * Semaphore::Down(TASK_CONTROL_BLOCK*)
	   * Locks multi-thread access to resources.
	   */
		void down(TASK_CONTROL_BLOCK* tcb) {
			if (sema_value > 0) {
				--sema_value;
			} else {
				scheduler->set_state(tcb, BLOCKED);
				semaphore_list.enqueue(tcb->task_id);
				
				pthread_mutex_lock(&mutex);
				pthread_cond_wait(&cond, &mutex);
			}
		}

		/*
	   * Semaphore::Down(int)
	   * Locks multi-thread access to resources.
	   */
		void down() {
			if (sema_value > 0) {
				--sema_value;
			} else {				
				pthread_mutex_lock(&mutex);
				pthread_cond_wait(&cond, &mutex);
			}
		}

		/*
	   * Semaphore::Up()
	   * Releases lock to resources.
	   */
	 	void up() {
			if (semaphore_list.size() > 0) {
				semaphore_list.dequeue();
				pthread_mutex_unlock(&mutex);
				pthread_cond_signal(&cond);
			} else {
				++sema_value;
			}
		}
};

#endif