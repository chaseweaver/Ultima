#include "inc/Semaphore.h"

/*
 * Semaphore::Semaphore(std::string, int)
 * Default constructor.
 */
Semaphore::Semaphore(MASTER_CONTROL_BLOCK* mcb, std::string name, int val)
	: master_control_block(mcb), resource_name(name), value(val) {}

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
  while (value == 0)
		cv.wait(lck);
	--value;
}

/*
 * Semaphore::wait(TASK_CONTROL_BLOCK*)
 * Locks threads depending on state.
 */
void Semaphore::wait(TASK_CONTROL_BLOCK* tcb) {
	std::unique_lock<std::mutex> lck(mtx);
	if (value == 0)
		sema_queue.enqueue(tcb);
  while (value == 0) {
		if (tcb->task_state != DEAD)
			set_state(tcb, BLOCKED);
		cv.wait(lck);
	}

	if (tcb->task_state != DEAD)
		set_state(tcb, RUNNING);
	--value;
}

/*
 * Semaphore::signal()
 * Signals to other threads that one has finished.
 */
void Semaphore::signal() {
	std::unique_lock<std::mutex> lck(mtx);
  ++value;
	if (!sema_queue.empty())
		sema_queue.dequeue();
  cv.notify_one();
}

/*
 * Semaphore::fetch_log()
 * Fetches contents of semaphore logs.
 */
std::string Semaphore::fetch_log() {
	std::string header = "\n Semaphore Log\n";
	std::string sema_title = " Title:";
	std::string sema_value_title = " Value:";
	std::string sema_list_title = " Queue:";

	pad(sema_title, 9, ' ');
	pad(sema_value_title, 9, ' ');
	pad(sema_list_title, 9, ' ');

	sema_title += resource_name;
	sema_value_title += std::to_string(value);

	if (!sema_queue.empty()) {
		Queue<TASK_CONTROL_BLOCK*>* tcb = new Queue<TASK_CONTROL_BLOCK*>(sema_queue);
		do {
			TASK_CONTROL_BLOCK* tmp;
			tcb->dequeue(tmp);
			sema_list_title += tcb->size() > 0 ? std::to_string(tmp->task_id) + " -> " : std::to_string(tmp->task_id);
		} while (!tcb->empty());
	}

	if (sema_list_title.length() == 9)
		sema_list_title = "There are no tasks in the queue.";

	return header + "\n" + sema_title + "\n" + sema_value_title + "\n" + sema_list_title;
}

/*
 * Semaphore::set_state(TASK_CONTROL_BLOCK*, int)
 * Changes task state in the list and logs to STATE WINDOW.
 * This is done inside the Semaphore to remove the need to re-call the scheduler.
 * For this project, the Semaphore needs to be coupled. This can be more-generic
 * outside of this.
 */
void Semaphore::set_state(TASK_CONTROL_BLOCK* tcb, int state) {
	if (tcb->task_state == state || tcb->task_state == DEAD)
		return;

	std::string str_old;
	switch (tcb->task_state) {
		case DEAD:
			str_old = "DEAD";
			break;
		case IDLE:
			str_old = "IDLE";
			break;
		case BLOCKED:
			str_old = "BLOCKED";
			break;
		case READY:
			str_old = "READY";
			break;
		case RUNNING:
			str_old = "RUNNING";
			break;
	}

	tcb->task_state = state;

	std::string str_new;
	switch (state) {
		case DEAD:
			str_new = "DEAD";
			break;
		case IDLE:
			str_new = "IDLE";
			break;
		case BLOCKED:
			str_new = "BLOCKED";
			break;
		case READY:
			str_new = "READY";
			break;
		case RUNNING:
			str_new = "RUNNING";
			break;
	}

	master_control_block->ui->write_refresh(STATE_WINDOW, " Thread #"
		+ std::to_string(tcb->task_id) + " " + str_old + " -> " + str_new + "\n");
	master_control_block->logger->add_log(tcb->task_id, tcb->task_name, tcb->task_state);
}