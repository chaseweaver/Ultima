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

		switch (wgetch(menu_window)) {
		case '0':
			endwin();
			exit(EXIT_SUCCESS);
			break;
		case '1':
			master_control_block->ui->clear_window(OUTPUT_WINDOW);
			print_log(OUTPUT_WINDOW);
			break;
		case '2':
			master_control_block->ui->clear_window(OUTPUT_WINDOW);
			print_tcb_semaphore(OUTPUT_WINDOW);
			break;
		case '3':
			master_control_block->ui->clear_window(MAILBOX_WINDOW);
			print_message_box_list(MAILBOX_WINDOW);
			break;
		}

		usleep(10000);
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
	master_control_block->ui->write_refresh(win, 2, 1, "Choose an option");
	master_control_block->ui->write_refresh(win, 2, 3, "1: System Logs");
	master_control_block->ui->write_refresh(win, 2, 4, "2: Semaphore Log");
	master_control_block->ui->write_refresh(win, 2, 5, "3: Mailbox List");
	master_control_block->ui->write_refresh(win, 2, 7, "0: Exit Program");
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
	master_control_block->ui->write_refresh(window_id, master_control_block->logger->fetch_log());
}

/*
 * Menu::print_tcb_semaphore(int)
 * Prints the TCB log to a specific window given ID. 
 */
void Menu::print_tcb_semaphore(int window_id) {
	master_control_block->ui->write_refresh(window_id, master_control_block->tcb_semaphore ->fetch_log());
}

/*
 * Menu::print_message_box_list(int)
 * Prints the IPC message box list to a specific window given ID. 
 */
void Menu::print_message_box_list(int window_id) {
	master_control_block->ui->write_refresh(window_id, master_control_block->ipc->fetch_message_box_list());
}