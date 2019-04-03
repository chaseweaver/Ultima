#include "../inc/Menu.h"

/*
 * Menu::Menu(MASTER_CONTROL_BLOCK*, WINDOW*, int)
 * Default constructor.
 */
Menu::Menu(MASTER_CONTROL_BLOCK* mcb, WINDOW* win) : mcb(mcb), menu_window(win) { start(); }

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
				mcb->ui->clear_window(OUTPUT_WINDOW);
				print_log(OUTPUT_WINDOW);
				mcb->ui->write_refresh(INPUT_WINDOW, " $ System Logs\n");
				break;

			case '2':
				mcb->ui->clear_window(OUTPUT_WINDOW);
				print_scheduler_logs(OUTPUT_WINDOW);
				mcb->ui->write_refresh(INPUT_WINDOW, " $ Scheduler Logs\n");
				break;

			case '3':
				mcb->ui->clear_window(OUTPUT_WINDOW);
				print_tcb_sema(OUTPUT_WINDOW);
				mcb->ui->write_refresh(INPUT_WINDOW, " $ Semaphore Logs\n");
				break;

			case '4':
				mcb->ui->clear_window(MAILBOX_WINDOW);
				print_message_box_list(MAILBOX_WINDOW);
				mcb->ui->write_refresh(INPUT_WINDOW, " $ Mailbox List\n");
				break;

			case '5':
				mcb->ui->clear_window(MENU_WINDOW);
				print_thread_inbox_menu(MENU_WINDOW);
				mcb->ui->write_refresh(INPUT_WINDOW, " $ Thread Inbox\n");

				input = wgetch(menu_window);

				do {
					if (input >= 49 && input <= 56) {
						mcb->ui->clear_window(MAILBOX_WINDOW);
						get_thread_message_box(MAILBOX_WINDOW, input - 48);
					}

					input = wgetch(menu_window);
					usleep(10000);
				} while (input != 27);

				mcb->ui->write_refresh(INPUT_WINDOW, " $ Return\n");
				mcb->ui->clear_window(MENU_WINDOW);
				print_menu(MENU_WINDOW);
				break;

			case '6':
				mcb->ui->clear_window(MENU_WINDOW);
				print_memory_mgmt_menu(MENU_WINDOW);
				mcb->ui->write_refresh(INPUT_WINDOW, " $ Memory Menu\n");

				input = wgetch(menu_window);

				do {

					if (input >= 49 && input <= 56) {
						mcb->ui->clear_window(MAILBOX_WINDOW);
						switch (input - 48) {

							// DUMP MANAGER QUEUE
							case 1:
								mcb->ui->write_refresh(MAILBOX_WINDOW, mcb->mem_man->memory_dump());
								mcb->ui->write_refresh(INPUT_WINDOW, " $ Mem Mgr Dump\n");
								break;

							// DUMP MAIN MEM
							case 2:
								mcb->ui->write_refresh(MAILBOX_WINDOW, mcb->mem_man->memory_dump_mem());
								mcb->ui->write_refresh(INPUT_WINDOW, " $ Main Mem Dump\n");
								break;

							// MEMEORY LEFT (NEEDS TO BE MADE)
							case 3:
								mcb->ui->write_refresh(MAILBOX_WINDOW,
																			 "The amount of un-allocated memory is: " +
																				 std::to_string(mcb->mem_man->memory_left()));
								mcb->ui->write_refresh(INPUT_WINDOW, " $ MEM LEFT\n");
								break;

							// LARGEST FREE BLOCK
							case 4:
								mcb->ui->write_refresh(MAILBOX_WINDOW,
																			 "The largest segment is: " +
																				 std::to_string(mcb->mem_man->memory_largest()));
								mcb->ui->write_refresh(INPUT_WINDOW, " $ Largest\n");
								break;

							// SMALLEST FREE BLOCK
							case 5:
								mcb->ui->write_refresh(MAILBOX_WINDOW,
																			 "The smallest segment is: " +
																				 std::to_string(mcb->mem_man->memory_smallest()));
								mcb->ui->write_refresh(INPUT_WINDOW, " $ Smallest\n");
								break;
						}
					}

					input = wgetch(menu_window);
					usleep(10000);
				} while (input != 27);

				mcb->ui->write_refresh(INPUT_WINDOW, " $ Return\n");
				mcb->ui->clear_window(MENU_WINDOW);
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
	static_cast< Menu* >(p)->menu();
	return NULL;
}

/*
 * Menu::print_menu(int)
 * Prints help menu for windows.
 */
void Menu::print_thread_menu(int win) {
	mcb->ui->write_refresh(win, "\n Select Window to spawn new thread:\n");
	mcb->ui->write_refresh(win, "\n 1: Window #1\t 5: Window #5");
	mcb->ui->write_refresh(win, "\n 2: Window #2\t 6: Window #6");
	mcb->ui->write_refresh(win, "\n 3: Window #3\t 7: Window #7");
	mcb->ui->write_refresh(win, "\n 4: Window #4\t 8: Window #8");
	mcb->ui->write_refresh(win, "\n\n ESC: Return");
}

/*
 * Menu::print_thread_inbox_menu(int)
 * Prints help menu for threads.
 */
void Menu::print_thread_inbox_menu(int win) {
	mcb->ui->write_refresh(win, "\n Select Thread to check inbox:\n");
	mcb->ui->write_refresh(win, "\n 1: Thread #1\t 5: Thread #5");
	mcb->ui->write_refresh(win, "\n 2: Thread #2\t 6: Thread #6");
	mcb->ui->write_refresh(win, "\n 3: Thread #3\t 7: Thread #7");
	mcb->ui->write_refresh(win, "\n 4: Thread #4\t 8: Thread #8");
	mcb->ui->write_refresh(win, "\n\n ESC: Return");
}

void Menu::print_memory_mgmt_menu(int win) {
	mcb->ui->write_refresh(win, "Select A Function To Run:\n");
	mcb->ui->write_refresh(win, "\n 1: Dump Manager Queue");
	mcb->ui->write_refresh(win, "\n 2: Dump Main Memory");
	mcb->ui->write_refresh(win, "\n 3: Show Memory Left Free");
	mcb->ui->write_refresh(win, "\n 4: Show Largest Memory");
	mcb->ui->write_refresh(win, "\n 5: Show Smallest Memory");
	mcb->ui->write_refresh(win, "\n\n ESC: Return");
}

/*
 * Menu::print_menu(int)
 * Prints help menu.
 */
void Menu::print_menu(int win) {
	mcb->ui->write_refresh(win, 2, 1, "Choose an option");
	mcb->ui->write_refresh(win, 2, 3, "1: System Logs   \t5: Check Thread Inbox");
	mcb->ui->write_refresh(win, 2, 4, "2: Scheduler Logs\t6: Memory Options");
	mcb->ui->write_refresh(win, 2, 5, "3: Semaphore Logs");
	mcb->ui->write_refresh(win, 2, 6, "4: Mailbox List");
	mcb->ui->write_refresh(win, 2, 7, "0: Exit Program");
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
void Menu::wait() { pthread_join(menu_thread, NULL); }

/*
 * Menu::print_log(int)
 * Prints the log to a specific window given ID.
 */
void Menu::print_log(int window_id) { mcb->ui->write_refresh(window_id, mcb->logger->fetch_log()); }

/*
 * Menu::print_scheduler_logs(int)
 * Prints the log to a specific window given ID.
 */
void Menu::print_scheduler_logs(int window_id) {
	mcb->ui->write_refresh(window_id, mcb->scheduler->fetch_log());
}

/*
 * Menu::print_tcb_sema(int)
 * Prints the TCB log to a specific window given ID.
 */
void Menu::print_tcb_sema(int window_id) {
	mcb->ui->write_refresh(window_id, mcb->tcb_sema->fetch_log());
}

/*
 * Menu::print_message_box_list(int)
 * Prints the IPC message box list to a specific window given ID.
 */
void Menu::print_message_box_list(int window_id) {
	mcb->ui->write_refresh(window_id, mcb->ipc->fetch_message_box_list());
}

void Menu::get_thread_message_box(int window_id, int thread_id) {
	mcb->ui->write_refresh(window_id, mcb->ipc->fetch_message_box_list(thread_id));
}
