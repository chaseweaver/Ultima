#ifndef MENU_H
#define MENU_H

#ifdef _MSC_VER
#pragma once
#endif

// Forward decleration
struct MASTER_CONTROL_BLOCK;

#include "MasterControlBlock.h"

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
	Menu(MASTER_CONTROL_BLOCK* mcb, WINDOW* win, int window_id)
		: master_control_block(mcb), menu_window(win), menu_window_id(window_id) {}
	~Menu() {}
	void print_menu(int);
	void set_menu_window(WINDOW*, int);
	void start();
	void stop();
	void wait();
	void print_log(int);
};

#endif