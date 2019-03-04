#include "UI.h"
#include "Scheduler.h"
#include <iostream>

UI ui;
Scheduler scheduler;

#define HEADING_WINDOW -1
#define LOG_WINDOW -2
#define STATE_WINDOW -3
#define CONSOLE_WINDOW -4
#define OUTPUT_WINDOW -5
#define SECRET_WINDOW -6

void heading_window();
void log_window();
void state_window();
void console_window();
void output_window();
void secret_window();

void* worker(void*);
void* secret_function(void*);

int main() {

	// Start UI Handler
	ui.start(ui);
	heading_window();
	log_window();
	state_window();
	console_window();
	output_window();
	secret_window();

	// Spawn child workers
	for (int i = 1; i <= 8; i++) {
		i <= 4 
			? ui.create_window_lock_spawn(" Worker #" + std::to_string(i)
				+ ' ', 4, 0, i, 19, 10, 3 + ((i - 1) * 20), 14)
			: ui.create_window_lock_spawn(" Worker #" + std::to_string(i)
				+ ' ', 4, 0, i, 19, 10, 3 + ((i - 5) * 20), 24);

		ARGUMENTS* args = scheduler.create_arguments(i, 0);
		scheduler.create_new_task("Worker #" + std::to_string(i), worker, args);
	}

	// Run secret function
	scheduler.create_new_task("Secret Function", secret_function, NULL);

	// Wait for UI thread to finish
	ui.wait();
	return 0;
}

/*
 * Ultima::heading_window()
 * Creates the initial heading window.
 */ 
void heading_window() {
	ui.create_window_spawn(HEADING_WINDOW, 79, 12, 3, 2);
	ui.write(HEADING_WINDOW, 26, 2, "ULTIMA 2.0 (Spring 2019)");
	ui.write(HEADING_WINDOW, 18, 3, "The Washington Redskins (Matt + Chase)");
	ui.write(HEADING_WINDOW, 2, 6, "$ Starting UI handler...");
	ui.write(HEADING_WINDOW, 2, 7, "$ Spawning child threads...");
}

/*
 * Ultima::log_window()
 * Creates the initial log window.
 */ 
void log_window() {
	ui.create_window_lock_spawn(" $ Log ", 2, 0, LOG_WINDOW, 39, 12, 3, 34);
}

/*
 * Ultima::state_window()
 * Creates the initial state window.
 */ 
void state_window() {
	ui.create_window_lock_spawn(" $ State ", 2, 0, STATE_WINDOW, 39, 12, 43, 34);
}

/*
 * Ultima::console_window()
 * Creates the initial console window.
 */ 
void console_window() {
	ui.create_window_lock_spawn(" $ Console ", 2, 0, CONSOLE_WINDOW, 50, 12, 83, 34);
	ui.write(CONSOLE_WINDOW, 16, 1, "Choose an option");
	ui.write(CONSOLE_WINDOW, 2, 3, "1: Log Threads");
	ui.write(CONSOLE_WINDOW, 2, 4, "2: Log Semaphore");
	ui.write(CONSOLE_WINDOW, 2, 5, "0: Exit Program");
}

/*
 * Ultima::output_window()
 * Creates the initial output window.
 */ 
void output_window() {
	ui.create_window_lock_spawn(" $ Output ", 2, 0, OUTPUT_WINDOW, 50, 32, 83, 2);
}

/*
 * Ultima::secret_window()
 * Creates the initial secret window.
 */ 
void secret_window() {
	ui.create_window_lock_spawn(" 8====D ", 2, 0, SECRET_WINDOW, 12, 44, 134, 2);
}

/*
 * Ultima::worker(void*)
 * Worker function to run in seperate threads.
 */ 
void* worker(void* arguments) {
	ARGUMENTS* args = (ARGUMENTS*) arguments;
	TASK_CONTROL_BLOCK* tcb = args->task_control_block;
	int& counter = args->thread_results;

	int r = counter + rand() % (10 + counter);
	sleep(1);
	do {
		ui.write(args->id, " Running #" + std::to_string(++counter) + "\n", tcb);
		ui.write(LOG_WINDOW, " Thread #" + std::to_string(args->id) + " is running #" + std::to_string(counter) + "\n");
		sleep(1);
	} while (counter != r);

	worker(args);

	//scheduler.set_state(tcb, DEAD);
	return NULL;
}

/*
 * Ultima::secret_function()
 * [CLASSIFIED]
 */ 
void* secret_function(void* arguments) {
	while (true) { ui.write(SECRET_WINDOW, "  8====D\n"); sleep(1); }
}