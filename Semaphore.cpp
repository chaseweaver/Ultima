#include "inc/Semaphore.h"

/*
 * Semaphore::down(TASK_CONTROL_BLOCK*)
 * Locks multi-thread access to resources.
 */
void Semaphore::down(TASK_CONTROL_BLOCK* tcb) {
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
void Semaphore::down(int tid) {
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
void Semaphore::up() {
	if (semaphore_list.size() > 0) {
		semaphore_list.wait_and_pop();
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
	} else {
		++sema_value;
	}
}