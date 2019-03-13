#ifndef MENU_H
#define MENU_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Struct.h"
#include <ncurses.h>

class Menu {
private:
	WINDOW* menu_window;
	bool enabled = false;
	pthread_t menu_thread;
	int menu_window_id;

	MASTER_CONTROL_BLOCK* master_control_block;

	void menu();
	static void* start_menu(void*);

public:
	Menu(MASTER_CONTROL_BLOCK*, WINDOW*, int);
	~Menu();
	void print_menu(int);
	void print_thread_menu(int);
	void set_menu_window(WINDOW*, int);
	void start();
	void stop();
	void wait();
	void print_log(int);
	void print_scheduler_logs(int);
	void print_tcb_semaphore(int);
	void print_message_box_list(int);
};

#endif