#include "inc/Semaphore.h"

/*
 * Semaphore::Semaphore(std::string, int)
 * Default constructor.
 */
Semaphore::Semaphore(std::string name, int val)
	: resource_name(name), value(val) {}

/*
 * Semaphore::~Semaphore()
 * Default deconstructor.
 */
Semaphore::~Semaphore() {}

/*
 * Semaphore::wait()
 * Locks threads depending on state.
 */
void Semaphore::wait() {
	pthread_mutex_lock(&mutex);

	while (value == 0)
		pthread_cond_wait(&cond, &mutex);
		
	--value;
	pthread_mutex_unlock(&mutex);
}

/*
 * Semaphore::wait(TASK_CONTROL_BLOCK*)
 * Locks threads depending on state.
 */
void Semaphore::wait(TASK_CONTROL_BLOCK* tcb) {
	pthread_mutex_lock(&mutex);

	while (value == 0) {
		tcb->task_state = BLOCKED;
		pthread_cond_wait(&cond, &mutex);
	}
		
	--value;
	pthread_mutex_unlock(&mutex);
}

/*
 * Semaphore::signal()
 * Signals to other threads that one has finished.
 */
void Semaphore::signal() {
	pthread_mutex_lock(&mutex);
	++value;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

/*
 * Logger::fetch_log()
 * Fetches contents of semaphore logs.
 */
std::string Semaphore::fetch_log() {
	std::string header = " Semaphore Log\n";
	std::string sema_title = " Title:";
	std::string sema_value_title = " Value:";
	std::string sema_list_title = " Queue:";

	pad(sema_title, 11, ' ');
	pad(sema_value_title, 11, ' ');
	pad(sema_list_title, 11, ' ');

	sema_title += resource_name;
	sema_value_title += std::to_string(value);

	if (!sema_queue.empty()) {
		Queue<TASK_CONTROL_BLOCK*>* tcb = new Queue<TASK_CONTROL_BLOCK*>(sema_queue);
		do {
			TASK_CONTROL_BLOCK* tmp;
			tcb->dequeue(tmp);
			sema_value_title += tcb->size() > 1 ? std::to_string(tmp->task_id) + " -> " : std::to_string(tmp->task_id);
		} while (!tcb->empty());
	}

	return header + "\n" + sema_title + "\n" + sema_value_title + "\n" + sema_list_title;
}