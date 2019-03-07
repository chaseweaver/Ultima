#ifndef UI_H
#define UI_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include "Scheduler.h"
#include "Struct.h"
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <string.h>

class UI {
private:
	struct WINDOW_OBJECT {
		std::string window_title;
		int window_id;
		int window_width;
		int window_height;
		int window_x;
		int window_y;
		WINDOW* window;
	};

	struct WINDOW_DATA {
		int window_id;
		int x;
		int y;
		std::string msg;
		TASK_CONTROL_BLOCK* task_control_block;
	};

	int refresh_rate = 60;
	bool enabled = false;
	pthread_t ui_thread;

	MASTER_CONTROL_BLOCK* master_control_block;
	Queue<WINDOW_DATA*>* window_data;
	Queue<WINDOW_OBJECT*>* window_object;

	void refresh();
	static void* start_refresh(void* p);
	WINDOW* fetch_window(int);
	bool write_window(WINDOW* win, std::string msg);
	bool write_window(WINDOW* win, int x, int y, std::string msg);
	bool write_window_refresh(WINDOW* win, std::string msg);
	bool write_window_refresh(WINDOW* win, int x, int y, std::string msg);

public:
	UI(MASTER_CONTROL_BLOCK* mcb);
	~UI();
	void start();
	void stop();
	void wait();
	bool is_running();
	void set_refresh_rate(int);
	int get_refresh_rate();
	WINDOW* create_window_spawn(std::string, int, int, int, int, int, int, int);
	WINDOW* create_window(std::string, int, int, int, int, int, int, int);
	WINDOW* create_window_spawn(std::string, int, int, int, int, int);
	WINDOW* create_window(std::string, int, int, int, int, int);
	WINDOW* create_window_spawn(int, int, int, int, int);
	WINDOW* create_window(int, int, int, int, int);
	WINDOW* create_window_lock_spawn(std::string, int, int, int, int, int, int, int);
	WINDOW* create_window_lock(std::string, int, int, int, int, int, int, int);
	void write(int, int, int, std::string, TASK_CONTROL_BLOCK*);
	void write(int, int, int, std::string);
	void write(int, std::string, TASK_CONTROL_BLOCK*);
	void write(int, std::string);
	void write_refresh(int, int, int, std::string, TASK_CONTROL_BLOCK*);
	void write_refresh(int, int, int, std::string);
	void write_refresh(int, std::string, TASK_CONTROL_BLOCK*);
	void write_refresh(int, std::string);
	void clear_window(WINDOW*);
	void clear_window(int);
	int get_message_list_size();
	int get_window_amount();
};

#endif