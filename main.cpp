#include "UI.h"
#include "Worker.h"
#include "Scheduler.h"
#include <iostream>

UI ui;
Worker worker = Worker(&ui);
Scheduler scheduler = Scheduler(&worker);

void heading_window();
void log_window();
void console_window();
void output_window();

int main() {

	// Start UI Handler
	ui.start(ui);
	heading_window();
	log_window();
	console_window();
	output_window();

	// Spawn child workers
	for (int i = 1; i <= 8; i++) {
		i <= 4 
			? ui.create_window_spawn(" Worker #" + std::to_string(i)
				+ ' ', 2, 0, 1, 19, 10, 3 + ((i - 1) * 20), 14)
			: ui.create_window_spawn(" Worker #" + std::to_string(i)
				+ ' ', 2, 0, 1, 19, 10, 3 + ((i - 5) * 20), 24);

			scheduler.create_new_task("Worker #" + std::to_string(i),
				worker.start, worker.create_arguments(i, 0));
	}

	// Wait for UI thread to finish
	ui.wait();
	return 0;
}

/*
 * Ultima::heading_window()
 * Creates the initial heading window.
 */ 
void heading_window() {
	ui.create_window_spawn(-1, 79, 12, 3, 2);
	ui.write(-1, 26, 2, "ULTIMA 2.0 (Spring 2019)");
	ui.write(-1, 18, 3, "The Washington Redskins (Matt + Chase)");
	ui.write(-1, 2, 6, "$ Starting UI handler...");
	ui.write(-1, 2, 7, "$ Spawning child threads...");
}

/*
 * Ultima::log_window()
 * Creates the initial log window.
 */ 
void log_window() {
	ui.create_window_spawn(" $ Log ", 2, 0, -2, 79, 12, 3, 34);
}

/*
 * Ultima::console_window()
 * Creates the initial console window.
 */ 
void console_window() {
	ui.create_window_spawn(" $ Console ", 2, 0, -3, 50, 12, 83, 34);
	ui.write(-3, 5, 2, "Choose an option");
	ui.write(-3, 2, 4, "1: Log Threads");
	ui.write(-3, 2, 5, "2: Log Semaphore");
	ui.write(-3, 2, 6, "0: Exit Program");
}

/*
 * Ultima::output_window()
 * Creates the initial output window.
 */ 
void output_window() {
	ui.create_window_spawn(" $ Output ", 2, 0, -4, 50, 32, 83, 2);
}