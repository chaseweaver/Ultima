#include "inc/Scheduler.h"

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
			//task_list.front(tcb);
			
			if (!task_list.try_and_pop(tcb))
				break;
			
			switch(tcb->task_state) {
				case DEAD:					
					yield(2, 7);
					break;

				case IDLE:
					yield(2, 7);
					break;

				case BLOCKED:
					set_state(tcb, READY);
					task_list.push(tcb);
					yield(2, 7);
					break;

				case READY:
					set_state(tcb, RUNNING);
					task_list.push(tcb);
					yield(2, 7);
					break;

				case RUNNING:
					set_state(tcb, READY);
					task_list.push(tcb);
					yield(2, 7);
					break;
			}

			task_list.push(tcb);
		}
	} while (true);
}

/*
 * Scheduler::Scheduler(MASTER_CONTROL_BLOCK*)
 * Default constructor. 
 */ 
Scheduler::Scheduler(MASTER_CONTROL_BLOCK* mcb) : master_control_block(mcb) {
	assert(!pthread_create(new pthread_t, NULL, start_scheduler, this));
}

/*
 * Scheduler::~Scheduler()
 * Default deconstructor. 
 */ 
Scheduler::~Scheduler() {}

/*
 * Scheduler::create_new_task(std::string, void* (void*), ARGUMENTS*)
 * Creates a new TASK_CONTROL_BLOCK and spawns a new child function in a new thread.
 */
void Scheduler::create_new_task(std::string task_name, void* worker(void*), ARGUMENTS* task_arguments) {
	TASK_CONTROL_BLOCK* tcb = new TASK_CONTROL_BLOCK;
	tcb->task_id = ++number_of_workers;
	tcb->task_state = READY;
	//task_list.empty() ? tcb->task_state = RUNNING : tcb->task_state = READY;
	tcb->task_name = task_name;
	tcb->task_thread = *(new pthread_t);
	task_arguments->task_control_block = tcb;
	tcb->task_arguments = task_arguments;

	task_list.push(tcb);
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
	return task_list.size();
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

	master_control_block->ui->write(STATE_WINDOW, " Thread #"
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
 * Scheduler::create_arguments(int, int)
 * Creates a new ARGUMENTS struct.
 */
ARGUMENTS* Scheduler::create_arguments(int id, int thread_results) {
	ARGUMENTS* args = new ARGUMENTS;
	args->id = id;
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
	args->thread_results = thread_results;
	args->task_control_block = task_control_block;
	return args;
}