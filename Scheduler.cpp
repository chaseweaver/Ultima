#include "inc/Scheduler.h"

/*
 * Scheduler::Scheduler(MASTER_CONTROL_BLOCK*)
 * Default constructor. 
 */ 
Scheduler::Scheduler(MASTER_CONTROL_BLOCK* mcb, int collector_timeout)
	: master_control_block(mcb), garbage_collector_timeout(collector_timeout) {
	assert(!pthread_create(&scheduler_thread, NULL, start_scheduler, this));
	assert(!pthread_create(&garbage_collector_thread, NULL, start_garbage_collector, this));
}

/*
 * Scheduler::~Scheduler()
 * Default deconstructor. 
 */ 
Scheduler::~Scheduler() {}

/*
 * Scheduler::yield(int, int)
 * Alternative to pthread_yield() since that is not cross-platform.
 */
void Scheduler::yield(int min, int max) {
	sleep(min + rand() % max);
}

/*
 * Scheduler::scheduler()
 * Function to handle state switching.
 */
void Scheduler::scheduler() {
	srand(time(NULL));
	do {
		if (!task_list.empty()) {
			TASK_CONTROL_BLOCK* tcb;
			task_list.front(tcb);

			//while (tcb->task_arguments->locked) sleep(1);

			switch(tcb->task_state) {
				case DEAD:
					break;

				// Unused state, potentially for the future.
				case IDLE:
					set_state(tcb, READY);
					break;

				case BLOCKED:
					yield(2, 2);
					set_state(tcb, READY);
					break;

				case READY:
					yield(2, 2);
					set_state(tcb, RUNNING);
					break;

				case RUNNING:
					yield(2, 2);
					set_state(tcb, READY);
					break;
			}

			task_list.enqueue_dequeue();
		}
	} while (true);
}

/*
 * Scheduler::garbage_collector()
 * Function to handle automatic garbage collecting switching.
 */
void Scheduler::garbage_collector() {
	return;
}

/*
 * Scheduler::create_new_task(std::string, void* (void*), ARGUMENTS*)
 * Creates a new TASK_CONTROL_BLOCK and spawns a new child function in a new thread.
 */
void Scheduler::create_new_task(std::string task_name, void* worker(void*), ARGUMENTS* task_arguments) {
	TASK_CONTROL_BLOCK* tcb = new TASK_CONTROL_BLOCK;
	tcb->task_id = ++number_of_workers;
	// tcb->task_state = READY;
	tcb->task_state = task_list.empty() ? RUNNING : READY;
	tcb->task_name = task_name;
	tcb->task_thread = *(new pthread_t);
	task_arguments->task_control_block = tcb;
	tcb->task_arguments = task_arguments;

	master_control_block->scheduler_semaphore->wait();
	task_list.enqueue(tcb);
	master_control_block->scheduler_semaphore->signal();

	assert(!pthread_create(&tcb->task_thread, NULL, worker, tcb->task_arguments));
}

void Scheduler::respawn(TASK_CONTROL_BLOCK* tcb, void* worker(void*), ARGUMENTS* task_arguments) {
	assert(!pthread_create(&tcb->task_thread, NULL, worker, tcb->task_arguments));
}

/*
 * Scheduler::task_list_size()
 * Returns the task_list size.
 */
int Scheduler::task_list_size() {
	master_control_block->scheduler_semaphore->wait();
	return task_list.size();
	master_control_block->scheduler_semaphore->signal();
}

/*
 * Scheduler::set_state(TASK_CONTROL_BLOCK*, int)
 * Changes task state in the list and logs to STATE WINDOW.
 */
void Scheduler::set_state(TASK_CONTROL_BLOCK* tcb, int state) {	
	if (tcb->task_state == state)
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

/*
 * Scheduler::set_state(int*, int)
 * Changes task state in the list and logs to STATE WINDOW.
 */
void Scheduler::set_state(int task_id, int state) {	
	TASK_CONTROL_BLOCK* tcb = get_task_control_block(task_id);

	if (tcb->task_id == state)
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

/*
 * Scheduler::start_scheduler(void*)
 * Starts the scheduler loop in a new thread.
 */
void* Scheduler::start_scheduler(void* p) {
	static_cast<Scheduler*>(p)->scheduler();
	return NULL;
}

/*
 * Scheduler::start_garbage_collector(void*)
 * Starts the garbage collector loop in a new thread.
 */
void* Scheduler::start_garbage_collector(void* p) {
	static_cast<Scheduler*>(p)->garbage_collector();
	return NULL;
}

/*
 * Scheduler::create_arguments(int, int)
 * Creates a new ARGUMENTS struct.
 */
ARGUMENTS* Scheduler::create_arguments(int id, int thread_results) {
	ARGUMENTS* args = new ARGUMENTS;
	args->id = id;
	args->locked = false;
	args->thread_results = thread_results;
	return args;
}

/*
 * Scheduler::create_arguments(int, int, TASK_CONTROL_BLOCK*)
 * Creates a new ARGUMENTS struct.
 */
ARGUMENTS* Scheduler::create_arguments(int id, int thread_results, TASK_CONTROL_BLOCK* task_control_block) {
	ARGUMENTS* args = new ARGUMENTS;
	args->id = id;
	args->locked = false;
	args->thread_results = thread_results;
	args->task_control_block = task_control_block;
	return args;
}

/*
 * Scheduler::get_task_control_block(int)
 * Returns a pointer to a thread's TASK_CONTROL_BLOCK given an ID.
 */
TASK_CONTROL_BLOCK* Scheduler::get_task_control_block(int tid) {
	Queue<TASK_CONTROL_BLOCK*>* tmp = new Queue<TASK_CONTROL_BLOCK*>(task_list);

	do {
		TASK_CONTROL_BLOCK* tcb;
		tmp->dequeue(tcb);

		if (tcb->task_id == tid)
			return tcb;

	} while (!tmp->empty());

	return NULL;
}