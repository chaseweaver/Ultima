#include "inc/Menu.h"

/*
 * Menu::Menu(MASTER_CONTROL_BLOCK*, WINDOW*, int)
 * Default constructor.
 */
Menu::Menu(MASTER_CONTROL_BLOCK* mcb, WINDOW* win, int window_id)
	: master_control_block(mcb), menu_window(win), menu_window_id(window_id) {
	start();
}

/*
 * Menu::~Menu()
 * Default deconstructor.
 */
Menu::~Menu() {}

void Menu::menu() {
	do {

		nodelay(menu_window, true);
		noecho();

		while (true) {
			int input = wgetch(menu_window);
			switch (input) {
				case '0':
				exit(0);
				break;
			case '1':
				master_control_block->ui->clear_window(OUTPUT_WINDOW);
				master_control_block->ui->write(OUTPUT_WINDOW, master_control_block->logger->fetch_log());
				break;
			}

			usleep(100000);
		}

		usleep(100000);
	} while (enabled);
}

/*
 * Menu::start_menu(void*)
 * Start menu handler in new thread.
 */
void* Menu::start_menu(void* p) {
	static_cast<Menu*>(p)->menu();
	return NULL;
}

/*
 * Menu::print_menu(int)
 * Prints help menu.
 */
void Menu::print_menu(int win) {
	master_control_block->ui->create_window_lock_spawn(" Console ", 2, 0, win, 60, 12, 83, 34);
	master_control_block->ui->write(win, 21, 1, "Choose an option");
	master_control_block->ui->write(win, 2, 3, "1: Log Threads");
	master_control_block->ui->write(win, 2, 4, "2: Log Semaphore");
	master_control_block->ui->write(win, 2, 5, "0: Exit Program");
}

/*
 * Menu::set_menu_window(WINDOW*, int)
 * Sets the menu window to write to.
 */ 
void Menu::set_menu_window(WINDOW* win, int window_id) {
	menu_window = win;
	menu_window_id = window_id;
}

/*
 * Menu::start()
 * Starts the menu handler in a new thread.
 */ 
void Menu::start() {
	if (!enabled) {
		enabled = true;
		assert(!pthread_create(&menu_thread, NULL, start_menu, this));
	}
}

/*
 * Menu::stop()
 * Enables the menu handler.
 */ 
void Menu::stop() {
	enabled = true;
	pthread_kill(menu_thread, 0);
}

/*
 * Menu::wait()
 * Waits for the menu thread to finish.
 */ 
void Menu::wait() {
	pthread_join(menu_thread, NULL);
}

/*
 * Menu::print_log(int)
 * Prints the log to a specific window given ID. 
 */
void Menu::print_log(int window_id) {
	master_control_block->ui->write(window_id, master_control_block->logger->fetch_log());
}