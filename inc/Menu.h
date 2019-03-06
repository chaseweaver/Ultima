#ifndef MENU_H
#define MENU_H

#ifdef _MSC_VER
#pragma once
#endif

#include "MasterControlBlock.h"
#include "ncurses.h"

class Menu {
private:
	WINDOW* menu_window;
	bool enabled = false;
	pthread_t menu_thread;
	int menu_window_id;

	void menu() {
		print_menu(menu_window_id);

		do {

			usleep(100000);
		} while (enabled);
	}

	/*
	* Menu::start_menu(void*)
	* Start menu handler in new thread
	*/
	static void* start_menu(void* p) {
		static_cast<Menu*>(p)->menu();
		return NULL;
	}

	void print_menu(int win) {
		master_control_block->ui->create_window_lock_spawn(" Console ", 2, 0, win, 60, 12, 83, 34);
		master_control_block->ui->write(win, 21, 1, "Choose an option");
		master_control_block->ui->write(win, 2, 3, "1: Log Threads");
		master_control_block->ui->write(win, 2, 4, "2: Log Semaphore");
		master_control_block->ui->write(win, 2, 5, "0: Exit Program");
	}

public:

	/*
	 * Menu::Menu(WINDOW*, int)
	 * Default constructor.
	 */ 
	Menu(WINDOW* win, int window_id) : menu_window(win) {
		print_menu(window_id);
		start();
	}

	/*
	 * Menu::~Menu()
	 * Default deconstructor.
	 */ 
	~Menu() {}

	/*
	 * Menu::set_menu_window(WINDOW*, int)
	 * Sets the menu window to write to.
	 */ 
	void set_menu_window(WINDOW* win, int window_id) {
		menu_window = win;
		menu_window_id = window_id;
	}

	/*
	 * Menu::start()
	 * Starts the menu handler in a new thread.
	 */ 
	void start() {
		if (!enabled) {
			enabled = true;
			assert(!pthread_create(&menu_thread, NULL, start_menu, this));
		}
	}

	/*
	 * Menu::stop()
	 * Enables the menu handler.
	 */ 
	void stop() {
		enabled = true;
		pthread_kill(menu_thread, 0);
	}

	/*
	 * Menu::wait()
	 * Waits for the menu thread to finish.
	 */ 
	void wait() {
		pthread_join(menu_thread, NULL);
	}

	/*
 	 * Menu::print_log(int)
	 * Prints the log to a specific window given ID.
	 */
	void print_log(int window_id) {
		master_control_block->ui->write(window_id, master_control_block->logger->fetch_log());
	}
};

#endif