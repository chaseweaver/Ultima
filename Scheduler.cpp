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
			task_list.wait_and_pop(tcb);
			task_list.push(tcb);

			switch(tcb->task_state) {
				case DEAD:
					break;

				case BLOCKED:
					set_state(tcb, READY);
					break;

				case READY:
					set_state(tcb, RUNNING);
					yield(2, 10);
					break;

				case RUNNING:
					break;
			}
		}
	} while (true);
}

/*
 * Scheduler::Scheduler()
 * Default constructor. 
 */ 
Scheduler::Scheduler() {
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
	tcb->task_name = task_name;
	tcb->task_thread = *(new pthread_t);
	tcb->task_arguments = task_arguments;

	task_list.push(tcb);
	assert(!pthread_create(&tcb->task_thread, NULL, worker, tcb->task_arguments));
}

/*
 * Scheduler::set_state(TASK_CONTROL_BLOCK*, int)
 * Changes task state in the list.
 */
void Scheduler::set_state(TASK_CONTROL_BLOCK* tcb, int state) {
	tcb->task_state = state;

	/*
	std::string task_state;
	switch(tcb->task_state) {
		case 0:
			task_state = "DEAD";
			break;
		case 1:
			task_state = "BLOCKED";
			break;
		case 2:
			task_state = "IDLE";
			break;
		case 3:
			task_state = "READY";
			break;
		case 4:
			task_state = "RUNNING";
			break;
	}
	
	mcb.ui->write(STATE_WINDOW, " Thread #" + std::to_string(tcb->task_id) + " is " + task_state + "\n");
	*/
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