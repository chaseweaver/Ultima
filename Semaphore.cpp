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
	std::unique_lock<std::mutex> lck(mtx);
  while (value == 0) cv.wait(lck);
	--value;
}

/*
 * Semaphore::wait(TASK_CONTROL_BLOCK*)
 * Locks threads depending on state.
 */
void Semaphore::wait(TASK_CONTROL_BLOCK* tcb) {
	std::unique_lock<std::mutex> lck(mtx);
  while (value == 0) { 
		tcb->task_state = BLOCKED;
		cv.wait(lck);
	}
	tcb->task_state = RUNNING;
	--value;
}

/*
 * Semaphore::signal()
 * Signals to other threads that one has finished.
 */
void Semaphore::signal() {
	std::unique_lock<std::mutex> lck(mtx);
  ++value;
  cv.notify_one();
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