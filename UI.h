#ifndef UI_H
#define UI_H

#ifdef _MSC_VER
#pragma once
#endif 

#include "LinkedList.h"
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <string.h>

class UI {
private:
	struct WINDOW_DATA {
		int window_id;
		int x;
		int y;
		std::string msg;
	};

	struct WINDOW_OBJECT {
		int window_id;
		WINDOW* window;
	};

	int refresh_rate = 60;
	bool enabled = false;
	pthread_t ui_thread;

	LinkedList<WINDOW_DATA*>* window_data;
	LinkedList<WINDOW_OBJECT*>* window_object;// = new LinkedList<WINDOW_OBJECT*>;

	/*
	 * UI::refresh()
	 * UI refresher loop. Refreshs windows that have events.
	 */
	void refresh() {
		do {

			// While at least one window exists and one event exists
			if (!window_data->is_empty() && !window_object->is_empty()) {
				WINDOW_DATA* win_dat = window_data->return_front();

				// Match the correct window and messsage ID
				// If match found, write to window given message and opt. (X, Y)
				int size = window_object->size();
				do {
					WINDOW_OBJECT* win_obj = window_object->return_front();

					if (win_obj->window_id != win_dat->window_id) {
						window_object->add(win_obj);
					} else {
						win_dat->x && win_dat->y ? write_window(win_obj->window, win_dat->x, win_dat->y, win_dat->msg)
							: write_window(win_obj->window, win_dat->msg);
						wrefresh(win_obj->window);
					}

					--size;
				} while (size != 0);
			}

			// Check this
			// Refresh the loop
			usleep((refresh_rate / 60) * 1000000);
		} while (enabled);
	}

	/*
	 * UI::start_refresher(void*)
	 * Start UI refresher  in new thread
	 */
	static void* start_refresher(void* p) {
		static_cast<UI*>(p)->refresh();
		return NULL;
	}

	/*
	 * UI::write_window(WINDOW*, std::string)
	 * Writes message to window. DOES NOT REFRESH.
	 */
	void write_window(WINDOW* win, std::string msg) {
		int len = msg.length();
		char char_array[len + 1];
		strcpy(char_array, msg.c_str());

		wprintw(win, char_array);
		box(win, 0 , 0);
	}

	/*
	 * UI::write_window(WINDOW*, int, int std::string)
	 * Writes message to window at (X, Y). DOES NOT REFRESH.
	 */
	void write_window(WINDOW* win, int x, int y, std::string msg) {
		int len = msg.length();
		char char_array[len + 1];
		strcpy(char_array, msg.c_str());

		mvwprintw(win, y, x, char_array);
		box(win, 0 , 0);
	} 

public:

	/*
	* UI::UI()
	* Default constructor. 
	*/ 
	UI() {
		window_data = new LinkedList<WINDOW_DATA*>;
		window_object = new LinkedList<WINDOW_OBJECT*>;
		initscr();
		refresh();
	}

	/*
	* UI::~UI()
	* Default deconstructor. 
	*/ 
	~UI() {
		endwin();
	}

	/*
	* UI::start()
	* Starts the UI refresher in a new thread;
	*/ 
	void start() {

		//std::cout << window_data->size();
		//std::cout << window_object->size();

		refresh();
		/*
		if (!enabled) {
			enabled = true;
			assert(!pthread_create(&ui_thread, NULL, start_refresher, NULL));
		}
		*/
	}

	/*
	* UI::stop()
	* Stops the UI refresher.
	*/ 
	void stop() {
		enabled = false;
	}

	/*
	* UI::is_running()
	* Returns true if the refresher is running, false otherwise.
	*/ 
	bool is_running() {
		return enabled;
	}

	/*
	* UI::set_refresh_rate(int)
	* Sets the refresh rate of the windows. 
	*/ 
	void set_refresh_rate(int rate) {
		refresh_rate = rate;
	}

	/*
	* UI::get_refresh_rate()
	* Returns the refresh rate of the windows. 
	*/ 
	int get_refresh_rate() {
		return refresh_rate;
	}

	/*
	* UI::add_window(int, int, int, int, int)
	* Adds a Curses window to the UI window object list.
	*/ 
	void add_window(int window_id, int width, int height, int x, int y) {
		WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
		WINDOW* win = newwin(height, width, y, x);
		scrollok(win, TRUE);
		scroll(win);
		box(win, 0, 0);

		win_obj->window_id = window_id;
		win_obj->window = win;
		window_object->add(win_obj);
	}
	
	/*
	* UI::write(int, int, int, std::string)
	* Writes a message to a window given ID and (X, Y).
	*/ 
	void write(int window_id, int x, int y, std::string msg) {
		WINDOW_DATA* win_dat = new WINDOW_DATA;
		win_dat->window_id = window_id;
		win_dat->x = x;
		win_dat->y = y;
		win_dat->msg = msg;
		window_data->add(win_dat);
	}

	/*
	* UI::write(int, std::string)
	* Writes a message to a window given ID.
	*/ 
	void write(int window_id, std::string msg) {
		WINDOW_DATA* win_dat = new WINDOW_DATA;
		win_dat->window_id = window_id;
		win_dat->msg = msg;
		window_data->add(win_dat);
	}

	/*
	* UI::get_message_queue_size()
	* Returns the amount of messages in the list.
	*/ 
	int get_message_list_size() {
		return window_data->size();
	}

	/*
	* UI::get_window_amount()
	* Returns amount of created windows.
	*/ 
	int get_window_amount() {
		return window_object->size();
	}
};

#endif