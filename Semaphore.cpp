#include "inc/Semaphore.h"

/*
 * Semaphore::down(TASK_CONTROL_BLOCK*)
 * Locks multi-thread access to resources.
 */
void Semaphore::down(TASK_CONTROL_BLOCK* tcb) {
	if (sema_value > 0) {
		--sema_value;
	} else {
		std::unique_lock<std::mutex> lock(m);

		master_control_block->scheduler->set_state(tcb, BLOCKED);
		semaphore_list->enqueue(tcb->task_id);
	}
	
	cond.notify_one();
}

/*
 * Semaphore::down(int)
 * Locks multi-thread access to resources.
 */
void Semaphore::down(int tid) {
	if (sema_value > 0) {
		--sema_value;
	} else {				
		std::lock_guard<std::mutex> lock(m);
		semaphore_list->enqueue(tid);
	}

	cond.notify_one();
}

/*
 * Semaphore::up()
 * Releases lock to resources.
 */
void Semaphore::up() {
	// std::unique_lock<std::mutex> lock(m);
	// while (semaphore_list->empty())
		// cond.wait(lock);

	if (semaphore_list->size() > 0) {

		semaphore_list->dequeue();

	} else {
		++sema_value;
	}
}