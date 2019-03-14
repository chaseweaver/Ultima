#include "../inc/Menu.h"

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
	int input;

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
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ System Logs\n");
			break;

		case '2':
			master_control_block->ui->clear_window(OUTPUT_WINDOW);
			print_scheduler_logs(OUTPUT_WINDOW);
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Scheduler Logs\n");
			break;

		case '3':
			master_control_block->ui->clear_window(OUTPUT_WINDOW);
			print_tcb_semaphore(OUTPUT_WINDOW);
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Semaphore Logs\n");
			break;

		case '4':
			master_control_block->ui->clear_window(MAILBOX_WINDOW);
			print_message_box_list(MAILBOX_WINDOW);
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Mailbox List\n");
			break;

		case '5':
			master_control_block->ui->clear_window(MENU_WINDOW);
			print_thread_menu(MENU_WINDOW);
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Spawn New Thread");
			
			input = wgetch(menu_window);

			do {
				if (input >= 49 && input <= 56)
					master_control_block->scheduler->create_new_task("Worker #"
						+ std::to_string(input - 48), master_control_block->worker->start_worker_function, 
						master_control_block->scheduler->create_arguments(input - 48, 0));

				input = wgetch(menu_window);
				usleep(10000);
			} while (input != 27);

			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Return\n");
			master_control_block->ui->clear_window(MENU_WINDOW);
			print_menu(MENU_WINDOW);
			break;

		case '6':
			master_control_block->ui->clear_window(MENU_WINDOW);
			print_thread_inbox_menu(MENU_WINDOW);
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Thread Inbox\n");
			
			input = wgetch(menu_window);

			do {
				if (input >= 49 && input <= 56) {
					master_control_block->ui->clear_window(MAILBOX_WINDOW);
					get_thread_message_box(MAILBOX_WINDOW, input - 48);
				}

				input = wgetch(menu_window);
				usleep(10000);
			} while (input != 27);

			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Return\n");
			master_control_block->ui->clear_window(MENU_WINDOW);
			print_menu(MENU_WINDOW);
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
 * Prints help menu for windows.
 */
void Menu::print_thread_menu(int win) {
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n Select Window to spawn new thread:\n");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 1: Window #1\t 5: Window #5");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 2: Window #2\t 6: Window #6");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 3: Window #3\t 7: Window #7");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 4: Window #4\t 8: Window #8");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n\n ESC: Return");
}

/*
 * Menu::print_thread_inbox_menu(int)
 * Prints help menu for threads.
 */
void Menu::print_thread_inbox_menu(int win) {
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n Select Thread to check inbox:\n");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 1: Thread #1\t 5: Thread #5");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 2: Thread #2\t 6: Thread #6");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 3: Thread #3\t 7: Thread #7");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n 4: Thread #4\t 8: Thread #8");
	master_control_block->ui->write_refresh(MENU_WINDOW, "\n\n ESC: Return");
}

/*
 * Menu::print_menu(int)
 * Prints help menu.
 */
void Menu::print_menu(int win) {
	master_control_block->ui->write_refresh(win, 2, 1, "Choose an option");
	master_control_block->ui->write_refresh(win, 2, 3, "1: System Logs   \t4: Mailbox list");
	master_control_block->ui->write_refresh(win, 2, 4, "2: Scheduler Logs\t5: Spawn New Thread");
	master_control_block->ui->write_refresh(win, 2, 5, "3: Semaphore Logs\t6: Check Thread Inbox");
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
 * Menu::print_scheduler_logs(int)
 * Prints the log to a specific window given ID. 
 */
void Menu::print_scheduler_logs(int window_id) {
	master_control_block->ui->write_refresh(window_id, master_control_block->scheduler->fetch_log());
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

void Menu::get_thread_message_box(int window_id, int thread_id) {
	master_control_block->ui->write_refresh(window_id, master_control_block->ipc->fetch_message_box_list(thread_id));
}