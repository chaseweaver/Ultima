#include "inc/UI.h"

/*
 * UI::refresher()
 * UI refresher loop. Refreshs windows that have events.
 */
void UI::refresh() {
	do {
		if (!window_data->empty()) {

			WINDOW_DATA* win_dat;
			window_data->wait_and_pop(win_dat);

			WINDOW_OBJECT* win_obj;
			bool success = false;

			while (!success && !window_object->empty()) {
				window_object->wait_and_pop(win_obj);

				if (win_obj->window_id == win_dat->window_id) {
					win_dat->x && win_dat->y
						? success = write_window_refresh(win_obj->window, win_dat->x, win_dat->y, win_dat->msg)
						: success = write_window_refresh(win_obj->window, win_dat->msg);
				}

				window_object->push(win_obj);
			}
		}

		// Refresh the loop based on refresh_rate
		usleep(100000);
	} while (enabled);
}

/*
 * UI::start_refresh(void*)
 * Start UI refresher in new thread
 */
void* UI::start_refresh(void* p) {
	static_cast<UI*>(p)->refresh();
	return NULL;
}

/*
 * UI::write_window(WINDOW*, std::string)
 * Writes message to window. DOES NOT REFRESH.
 */
bool UI::write_window(WINDOW* win, std::string msg) {
	int len = msg.length();
	char char_array[len + 1];
	strcpy(char_array, msg.c_str());

	wprintw(win, char_array);
	return true;
}

/*
 * UI::write_window(WINDOW*, int, int std::string)
 * Writes message to window at (X, Y). DOES NOT REFRESH.
 */
bool UI::write_window(WINDOW* win, int x, int y, std::string msg) {
	int len = msg.length();
	char char_array[len + 1];
	strcpy(char_array, msg.c_str());

	mvwprintw(win, y, x, char_array);
	return true;
}

/*
 * UI::write_window_refresh(WINDOW*, std::string)
 * Writes message to window. REFRESHES
 */
bool UI::write_window_refresh(WINDOW* win, std::string msg) {
	int len = msg.length();
	char char_array[len + 1];
	strcpy(char_array, msg.c_str());

	wprintw(win, char_array);
	wrefresh(win);
	return true;
}

/*
 * UI::write_window_refresh(WINDOW*, int, int std::string)
 * Writes message to window at (X, Y). REFRESHES
 */
bool UI::write_window_refresh(WINDOW* win, int x, int y, std::string msg) {
	int len = msg.length();
	char char_array[len + 1];
	strcpy(char_array, msg.c_str());

	mvwprintw(win, y, x, char_array);
	wrefresh(win);
	return true;
}

/*
 * UI::UI()
 * Default constructor. 
 */ 
UI::UI() {
	window_data = new Queue<WINDOW_DATA*>;
	window_object = new Queue<WINDOW_OBJECT*>;
	initscr();
	refresh();

	start();
}

/*
 * UI::~UI()
 * Default deconstructor. 
 */ 
UI::~UI() {
	endwin();
}

/*
 * UI::start()
 * Starts the UI refresher in a new thread;
 */ 
void UI::start() {
	if (!enabled) {
		enabled = true;
		assert(!pthread_create(&ui_thread, NULL, start_refresh, this));
	}
}

/*
 * UI::stop()
 * Stops the UI refresher.
 */
void UI::stop() {
	enabled = false;
	pthread_kill(ui_thread, 0);
}

/*
 * UI::wait()
 * Waits for the UI thread to finish.
 */ 
void UI::wait() {
	pthread_join(ui_thread, NULL);
}

/*
 * UI::is_running()
 * Returns true if the refresher is running, false otherwise.
 */ 
bool UI::is_running() {
	return enabled;
}

/*
 * UI::set_refresh_rate(int)
 * Sets the refresh rate of the windows. 
 */ 
void UI::set_refresh_rate(int rate) {
	refresh_rate = rate;
}

/*
 * UI::get_refresh_rate()
 * Returns the refresh rate of the windows. 
 */ 
int UI::get_refresh_rate() {
	return refresh_rate;
}

/*
 * UI::create_window_spawn(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window_spawn(std::string title, int title_x, int title_y,
	int window_id, int width, int height, int x, int y) {

	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	scrollok(win, true);
	scroll(win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = win;
	
	window_object->push(win_obj);
	write_window(win, title_x, title_y, title);
	wrefresh(win);
	return win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window(std::string title, int title_x, int title_y,
	int window_id, int width, int height, int x, int y) {

	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	scrollok(win, true);
	scroll(win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = win;

	window_object->push(win_obj);
	write_window(win, title_x, title_y, title);
	return win;
}

/*
 * UI::create_window_spawn(std::string, int, int, int, int, int)
 * Adds a Curses window with a title to the UI window object list and SPAWNS it.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window_spawn(std::string title, int window_id, int width, int height, int x, int y) {
	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	WINDOW* write_win = newwin(height - 4, width - 3, y + 3, x + 2);
	scrollok(win, true);
	scroll(win);
	scrollok(write_win, true);
	scroll(write_win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = write_win;

	window_object->push(win_obj);
	int offset_x = (width / 2) - (title.length() / 2);
	write_window(win, offset_x, 1, title);
	wrefresh(win);
	wrefresh(write_win);
	return write_win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int)
 * Adds a Curses window with a title to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window(std::string title, int window_id, int width, int height, int x, int y) {
	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	WINDOW* write_win = newwin(height - 4, width - 3, y + 3, x + 2);
	scrollok(win, true);
	scroll(win);
	scrollok(write_win, true);
	scroll(write_win);
	box(win, 0, 0);
	box(write_win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = write_win;

	window_object->push(win_obj);
	int offset_x = (width / 2) - (title.length() / 2);
	write_window(win, offset_x, 1, title);
	return write_win;
}

/*
 * UI::create_window_spawn(int, int, int, int, int)
 * Adds a Curses window to the UI window object list and SPAWNS it.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window_spawn(int window_id, int width, int height, int x, int y) {
	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	scrollok(win, TRUE);
	scroll(win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = win;

	window_object->push(win_obj);
	wrefresh(win);	
	return win;
}

/*
 * UI::create_window(int, int, int, int, int)
 * Adds a Curses window to the UI window object list.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window(int window_id, int width, int height, int x, int y) {
	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	scrollok(win, TRUE);
	scroll(win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = win;

	window_object->push(win_obj);
	return win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list and SPAWNS it.
 * Locks window within another.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window_lock_spawn(std::string title, int title_x, int title_y,
	int window_id, int width, int height, int x, int y) {

	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	WINDOW* write_win = newwin(height - 2, width - 2, y + 1, x + 1);
	scrollok(win, true);
	scroll(win);
	scrollok(write_win, true);
	scroll(write_win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = write_win;

	window_object->push(win_obj);
	write_window(win, title_x, title_y, title);
	wrefresh(win);
	wrefresh(write_win);
	return write_win;
}

/*
 * UI::create_window(std::string, int, int, int, int, int, int, int)
 * Adds a Curses window with a title (X, Y) to the UI window object list.
 * Locks window within another.
 * Returns a pointer to the WINDOW object.
 */ 
WINDOW* UI::create_window_lock(std::string title, int title_x, 
	int title_y, int window_id, int width, int height, int x, int y) {

	WINDOW_OBJECT* win_obj = new WINDOW_OBJECT;
	WINDOW* win = newwin(height, width, y, x);
	WINDOW* write_win = newwin(height - 2, width - 2, y + 1, x + 1);
	scrollok(win, true);
	scroll(win);
	scrollok(write_win, true);
	scroll(write_win);
	box(win, 0, 0);

	win_obj->window_id = window_id;
	win_obj->window_width = width;
	win_obj->window_height = height;
	win_obj->window_x = x;
	win_obj->window_y = y;
	win_obj->window = write_win;

	window_object->push(win_obj);
	write_window(win, title_x, title_y, title);	
	return write_win;
}

/*
 * UI::write(int, int, int, std::string, TASK_CONTROL_BLOCK*)
 * Writes a message to a window given ID and (X, Y) and stores TASK ID.
 */ 
void UI::write(int window_id, int x, int y, std::string msg, TASK_CONTROL_BLOCK* tcb) {
	WINDOW_DATA* win_dat = new WINDOW_DATA;
	win_dat->window_id = window_id;
	win_dat->x = x;
	win_dat->y = y;
	win_dat->msg = msg;
	win_dat->task_control_block = tcb;
	window_data->push(win_dat);
}

/*
 * UI::write(int, int, int, std::string)
 * Writes a message to a window given ID and (X, Y).
 */ 
void UI::write(int window_id, int x, int y, std::string msg) {
	WINDOW_DATA* win_dat = new WINDOW_DATA;
	win_dat->window_id = window_id;
	win_dat->x = x;
	win_dat->y = y;
	win_dat->msg = msg;
	window_data->push(win_dat);
}

/*
 * UI::write(int, std::string, TASK_CONTROL_BLOCK*)
 * Writes a message to a window given ID and stores TASK CONTROL BLOCK.
 */ 
void UI::write(int window_id, std::string msg, TASK_CONTROL_BLOCK* tcb) {
	WINDOW_DATA* win_dat = new WINDOW_DATA;
	win_dat->window_id = window_id;
	win_dat->msg = msg;
	win_dat->task_control_block = tcb;
	window_data->push(win_dat);
}

/*
 * UI::write(int, std::string)
 * Writes a message to a window given ID.
 */ 
void UI::write(int window_id, std::string msg) {
	WINDOW_DATA* win_dat = new WINDOW_DATA;
	win_dat->window_id = window_id;
	win_dat->msg = msg;
	window_data->push(win_dat);
}

/*
 * UI::get_message_list_size()
 * Returns the amount of messages in the list.
 */ 
int UI::get_message_list_size() {
	return window_data->size();
}

/*
 * UI::get_window_amount()
 * Returns amount of created windows.
 */ 
int UI::get_window_amount() {
	return window_object->size();
}