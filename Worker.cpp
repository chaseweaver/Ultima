#include "inc/Worker.h"

/*
 * Worker::worker(MASTER_CONTROL_BLOCK*)
 * Default constructor.
 */
Worker::Worker(MASTER_CONTROL_BLOCK* mcb) : master_control_block(mcb) {}

/*
 * Worker::~worker()
 * Default deconstructor.
 */
Worker::~Worker() {}

/*
 * Worker::start_worker_function(void*)
 * Helper function to start the worker.
 */
void* Worker::start_worker_function(void* p) {
	static_cast<Worker*>(p)->worker_function(p);
	return NULL;
}

/*
 * Worker::worker_function(void*)
 * Worker function to run in seperate threads.
 */
void* Worker::worker_function(void* arguments) {
	ARGUMENTS* args = (ARGUMENTS*) arguments;
	TASK_CONTROL_BLOCK* tcb = args->task_control_block;
	int& counter = args->thread_results;

	int r = 1 + rand() % 100;
	do {
		master_control_block->tcb_semaphore->wait(tcb);

		while (tcb->task_state == RUNNING) {
			if (counter == r)
				break;

			// Just for example, we have the workers let other workers know when they are half done.
			if (counter == r / 2) {
				int tmp_rand = 1 + rand() % 8;
				int result = master_control_block->ipc->message_send(
					master_control_block->ipc->compose_message(tcb, tmp_rand, "I am half way done!")
				);

				result == 1
					? master_control_block->ui->write_refresh(args->id,
							"\n Message sent\n to Thread #" + std::to_string(tmp_rand) + "\n\n")
					:	master_control_block->ui->write_refresh(args->id,
							"\n Message failed\n to send.\n\n");
			}

			master_control_block->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
			master_control_block->ui->write_refresh(LOG_WINDOW, " Thread #" + std::to_string(args->id) 
				+ " is running #" + std::to_string(counter) + "\n");
			
			usleep(100000);
		}

		master_control_block->tcb_semaphore->signal();

		sleep(1);
	} while (counter != r);

	master_control_block->ui->write_refresh(args->id, "\n Thread #"
		+ std::to_string(args->id) + "\n has ended.\n");
	master_control_block->ui->write_refresh(LOG_WINDOW, " Thread #"
		+ std::to_string(args->id) + " has ended.\n");

	master_control_block->scheduler->set_state(tcb, DEAD);
	return NULL;
}