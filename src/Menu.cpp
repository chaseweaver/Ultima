#include "../inc/Menu.h"

/*
 * Menu::Menu(MASTER_CONTROL_BLOCK*, WINDOW*, int)
 * Default constructor.
 */
Menu::Menu(MASTER_CONTROL_BLOCK* mcb, WINDOW* win)
	: master_control_block(mcb), menu_window(win) {
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

		case '6':
			master_control_block->ui->clear_window(MENU_WINDOW);
			print_memory_mgmt_menu(MENU_WINDOW);
			master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Memory Menu\n");

			input = wgetch(menu_window);

			do {
				
				if (input >= 49 && input <= 56) {
					master_control_block->ui->clear_window(MAILBOX_WINDOW);
					switch (input - 48)
					{
						//DUMP MANAGER QUEUE
						case 1:
							master_control_block->ui->write_refresh(MAILBOX_WINDOW, master_control_block->memory_manager->memory_dump());
							master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Mem Mgr Dump\n");
							break;
						//DUMP MAIN MEM
						case 2:
							master_control_block->ui->write_refresh(MAILBOX_WINDOW, master_control_block->memory_manager->memory_dump_mem());
							master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Main Mem Dump\n");
							break;
						//MEMEORY LEFT (NEEDS TO BE MADE)
						case 3:
							master_control_block->ui->write_refresh(MAILBOX_WINDOW, std::to_string(master_control_block->memory_manager->memory_left()));
							master_control_block->ui->write_refresh(INPUT_WINDOW, " $ MEM LEFT\n");
							break;
						//LARGEST FREE BLOCK
						case 4:
							master_control_block->ui->write_refresh(MAILBOX_WINDOW, std::to_string(master_control_block->memory_manager->memory_largest()));
							master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Largest\n");
							break;
						//SMALLEST FREE BLOCK
						case 5:
							master_control_block->ui->write_refresh(MAILBOX_WINDOW, std::to_string(master_control_block->memory_manager->memory_smallest()));
							master_control_block->ui->write_refresh(INPUT_WINDOW, " $ Smallest\n");
							break;
					
						default:
							break;
					}
					
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
	master_control_block->ui->write_refresh(win, "\n Select Window to spawn new thread:\n");
	master_control_block->ui->write_refresh(win, "\n 1: Window #1\t 5: Window #5");
	master_control_block->ui->write_refresh(win, "\n 2: Window #2\t 6: Window #6");
	master_control_block->ui->write_refresh(win, "\n 3: Window #3\t 7: Window #7");
	master_control_block->ui->write_refresh(win, "\n 4: Window #4\t 8: Window #8");
	master_control_block->ui->write_refresh(win, "\n\n ESC: Return");
}

/*
 * Menu::print_thread_inbox_menu(int)
 * Prints help menu for threads.
 */
void Menu::print_thread_inbox_menu(int win) {
	master_control_block->ui->write_refresh(win, "\n Select Thread to check inbox:\n");
	master_control_block->ui->write_refresh(win, "\n 1: Thread #1\t 5: Thread #5");
	master_control_block->ui->write_refresh(win, "\n 2: Thread #2\t 6: Thread #6");
	master_control_block->ui->write_refresh(win, "\n 3: Thread #3\t 7: Thread #7");
	master_control_block->ui->write_refresh(win, "\n 4: Thread #4\t 8: Thread #8");
	master_control_block->ui->write_refresh(win, "\n\n ESC: Return");
}

void Menu::print_memory_mgmt_menu(int win){
	master_control_block->ui->write_refresh(win, "Select A Function To Run:\n");
	master_control_block->ui->write_refresh(win, "\n 1: Dump Manager Queue");
	master_control_block->ui->write_refresh(win, "\n 2: Dump Main Memory");
	master_control_block->ui->write_refresh(win, "\n 3: Show Memory Left Free");
	master_control_block->ui->write_refresh(win, "\n 4: Show Largest Memory");
	master_control_block->ui->write_refresh(win, "\n 5: Show Smallest Memory");
	master_control_block->ui->write_refresh(win, "\n\n ESC: Return");
}

/*
 * Menu::print_menu(int)
 * Prints help menu.
 */
void Menu::print_menu(int win) {
	master_control_block->ui->write_refresh(win, 2, 1, "Choose an option");
	master_control_block->ui->write_refresh(win, 2, 3, "1: System Logs   \t5: Check Thread Inbox");
	master_control_block->ui->write_refresh(win, 2, 4, "2: Scheduler Logs\t6: Memory Options");
	master_control_block->ui->write_refresh(win, 2, 5, "3: Semaphore Logs");
	master_control_block->ui->write_refresh(win, 2, 6, "4: Mailbox List");
	master_control_block->ui->write_refresh(win, 2, 7, "0: Exit Program");
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
	enabled = false;
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