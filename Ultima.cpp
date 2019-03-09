#include "inc/MasterControlBlock.h"

void master_control_block_init();
void heading_window(int);
void log_window(int);
void state_window(int);
void console_window(int);
void output_window(int);
void mailbox_window(int);
void secret_window(int);
void* worker(void*);

MASTER_CONTROL_BLOCK* master_control_block = new MASTER_CONTROL_BLOCK;

int main() {
	
	master_control_block_init();
	heading_window(HEADING_WINDOW);
	log_window(LOG_WINDOW);
	state_window(STATE_WINDOW);
	console_window(CONSOLE_WINDOW);
	output_window(OUTPUT_WINDOW);
	mailbox_window(MAILBOX_WINDOW);

	// Spawn child workers
	for (int i = 1; i <= 8; i++) {
		i <= 4 
			? master_control_block->ui->create_window_lock_spawn(" Worker #" + std::to_string(i)
				+ ' ', 4, 0, i, 19, 10, 3 + ((i - 1) * 20), 14)
			: master_control_block->ui->create_window_lock_spawn(" Worker #" + std::to_string(i)
		 		+ ' ', 4, 0, i, 19, 10, 3 + ((i - 5) * 20), 24);
		master_control_block->scheduler->create_new_task("Worker #"
			+ std::to_string(i), worker, master_control_block->scheduler->create_arguments(i, 0));
	}

	// Wait for UI thread to finish
	// master_control_block->ui->wait();
	master_control_block->menu->wait();
	return 0;
}

/*
 * Ultima::master_control_block_init()
 * Initializes the MASTER_CONTROL_BLOCK objects.
 */ 
void master_control_block_init() {
	master_control_block->scheduler = new Scheduler(master_control_block, 5);
	master_control_block->ui_semaphore = new Semaphore("UI Handler", 1);
	master_control_block->scheduler_semaphore = new Semaphore("Scheduler Handler", 1);
	master_control_block->logger_semaphore = new Semaphore("Logger Handler", 1);
	master_control_block->tcb_semaphore = new Semaphore("TCB Locker", 1);
	master_control_block->ui = new UI(master_control_block);
	master_control_block->logger = new Logger(32);
}

/*
 * Ultima::heading_window(int)
 * Creates the initial heading window.
 */ 
void heading_window(int win) {
	master_control_block->ui->create_window_spawn(win, 79, 12, 3, 2);
	master_control_block->ui->write(win, 26, 2, "ULTIMA 2.0 (Spring 2019)");
	master_control_block->ui->write(win, 18, 3, "The Washington Redskins (Matt + Chase)");
	master_control_block->ui->write(win, 2, 6, "$ Starting UI handler...");
	master_control_block->ui->write_refresh(win, 2, 7, "$ Spawning child threads...");
}

/*
 * Ultima::log_window(int)
 * Creates the initial log window.
 */
void log_window(int win) {
	master_control_block->ui->create_window_lock_spawn(" Log ", 2, 0, win, 39, 12, 3, 34);
}

/*
 * Ultima::state_window(int)
 * Creates the initial state window.
 */ 
void state_window(int win) {
	master_control_block->ui->create_window_lock_spawn(" State ", 2, 0, win, 39, 12, 43, 34);
}

/*
 * Ultima::console_window(int)
 * Creates the initial console window.
 */ 
void console_window(int win) {
	master_control_block->menu = new Menu(master_control_block,
		master_control_block->ui->create_window_lock_spawn
		(" Console ", 2, 0, win, 60, 12, 83, 34), win);
	master_control_block->menu->print_menu(win);
}

/*
 * Ultima::output_window(int)
 * Creates the initial output window.
 */ 
void output_window(int win) {
	master_control_block->ui->create_window_lock_spawn(" Output ", 2, 0, win, 60, 12, 83, 2);
}

/*
 * Ultima::mailbox_window(int)
 * Creates the initial output window.
 */ 
void mailbox_window(int win) {
	master_control_block->ui->create_window_lock_spawn(" Mailbox ", 2, 0, win, 60, 20, 83, 14);
}

/*
 * Ultima::worker(void*)
 * Worker function to run in seperate threads.
 */
void* worker(void* arguments) {
	ARGUMENTS* args = (ARGUMENTS*) arguments;
	TASK_CONTROL_BLOCK* tcb = args->task_control_block;
	int& counter = args->thread_results;

	int r = 1 + rand() % 100;
	do {

		while (tcb->task_state == RUNNING) {
			if (counter == r)
				break;

			master_control_block->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n", tcb);
			master_control_block->ui->write_refresh(LOG_WINDOW, " Thread #" + std::to_string(args->id)
				+ " is running #" + std::to_string(counter) + "\n");
			usleep(1000000);
		}

		sleep(1);
	} while (counter != r);

	master_control_block->ui->write_refresh(args->id, "\n Thread #"
		+ std::to_string(args->id) + "\n has ended.\n");
	master_control_block->ui->write_refresh(LOG_WINDOW, " Thread #"
		+ std::to_string(args->id) + " has ended.\n");

	master_control_block->scheduler->set_state(tcb, DEAD);
	return NULL;
}