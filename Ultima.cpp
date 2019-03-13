#include "inc/MasterControlBlock.h"

void master_control_block_init();
void window_init();
void* worker_function(void*);

MASTER_CONTROL_BLOCK* master_control_block = new MASTER_CONTROL_BLOCK;

int NUMBER_OF_WORKERS = 8;

int main() {
	
	master_control_block_init();
	window_init();

	// Spawn child workers
	for (int i = 1; i <= NUMBER_OF_WORKERS; i++) {
		i <= 4 
			? master_control_block->ui->create_window_lock_spawn(" Worker #" + std::to_string(i)
				+ ' ', 4, 0, i, 19, 10, 3 + ((i - 1) * 20), 14)
			: master_control_block->ui->create_window_lock_spawn(" Worker #" + std::to_string(i)
		 		+ ' ', 4, 0, i, 19, 10, 3 + ((i - 5) * 20), 24);
		master_control_block->scheduler->create_new_task("Worker #"
			+ std::to_string(i), worker_function, master_control_block->scheduler->create_arguments(i, 0));
	}

	// Wait for UI thread to finish
	// master_control_block->ui->wait();
	master_control_block->menu->wait();
	return 0;
}

/*
 * Ultima::master_control_block_init()
 * Initializes the MASTER_CONTROL_BLOCK objects.
 */ 
void master_control_block_init() {
	master_control_block->ui_semaphore = new Semaphore(master_control_block, "UI Handler", 1);
	master_control_block->scheduler_semaphore = new Semaphore(master_control_block, "Scheduler Handler", 1);
	master_control_block->logger_semaphore = new Semaphore(master_control_block, "Logger Handler", 1);
	master_control_block->tcb_semaphore = new Semaphore(master_control_block, "TCB Locker", 1);
	master_control_block->ipc_semaphore = new Semaphore(master_control_block, "IPC Handler", 1);
	master_control_block->scheduler = new Scheduler(master_control_block, 5);
	master_control_block->ui = new UI(master_control_block);
	master_control_block->logger = new Logger(32);
	master_control_block->ipc = new IPC(master_control_block, NUMBER_OF_WORKERS, 8);
	master_control_block->worker = new Worker(master_control_block);
}

/*
 * Ultima::window_init()
 * Creates the initial windows.
 */ 
void window_init() {
	master_control_block->ui->create_window_spawn(HEADING_WINDOW, 79, 12, 3, 2);
	master_control_block->ui->write(HEADING_WINDOW, 26, 2, "ULTIMA 2.0 (Spring 2019)");
	master_control_block->ui->write(HEADING_WINDOW, 18, 3, "The Washington Redskins (Matt + Chase)");
	master_control_block->ui->write(HEADING_WINDOW, 2, 6, "$ Starting UI handler...");
	master_control_block->ui->write_refresh(HEADING_WINDOW, 2, 7, "$ Spawning child threads...");

	master_control_block->ui->create_window_lock_spawn(" Log ", 2, 0, LOG_WINDOW, 39, 12, 3, 34);
	master_control_block->ui->create_window_lock_spawn(" State ", 2, 0, STATE_WINDOW, 39, 12, 43, 34);
	master_control_block->ui->create_window_lock_spawn(" Output ", 2, 0, OUTPUT_WINDOW, 80, 12, 83, 2);
	master_control_block->ui->create_window_lock_spawn(" Mailbox ", 2, 0, MAILBOX_WINDOW, 80, 20, 83, 14);
	master_control_block->ui->create_window_lock_spawn(" Input ", 2, 0, INPUT_WINDOW, 21, 12, 142, 34);
	master_control_block->ui->write(INPUT_WINDOW, "\n");

	master_control_block->menu = new Menu(master_control_block,
		master_control_block->ui->create_window_lock_spawn
		(" Menu ", 2, 0, MENU_WINDOW, 58, 12, 83, 34), MENU_WINDOW);
	master_control_block->menu->print_menu(MENU_WINDOW);
}


/*
 * Ultima::worker_function(void*)
 * Worker function to run in seperate threads.
 * It also tells jokes.
 */
void* worker_function(void* arguments) {
	ARGUMENTS* args = (ARGUMENTS*) arguments;
	TASK_CONTROL_BLOCK* tcb = args->task_control_block;
	int& counter = args->thread_results;

	std::string message_lists[17] = {
		"Did you hear about the restaurant on the moon? Great food, no atmosphere.",
		"What do you call a fake noodle? An Impasta.",
		"How many apples grow on a tree? All of them.",
		"Want to hear a joke about paper? Nevermind it's tearable.",
		"Why did the coffee file a police report? It got mugged.",
		"How does a penguin build it's house? Igloos it together.",
		"Dad, did you get a haircut? No I got them all cut.",
		"Why did the scarecrow win an award? Because he was outstanding in his field.",
		"What do you call an elephant that doesn't matter? An irrelephant",
		"What do you call cheese that isn't yours? Nacho Cheese.",
		"What did the grape do when he got stepped on? He let out a little wine.",
		"I would avoid the sushi if I was you. It’s a little fishy.",
		"What's brown and sticky? A stick.",
		"I thought about going on an all-almond diet. But that's just nuts",
		"People don't like having to bend over to get their drinks. We really need to raise the bar.",
		"I don’t play soccer because I enjoy the sport. I’m just doing it for kicks.",
		"Why do you never see elephants hiding in trees? Because they're so good at it."
	};

	int num = 1 + rand() % 100;
	do {	
		master_control_block->tcb_semaphore->wait(tcb);

		while (tcb->task_state == RUNNING) {
			if (counter == num)
				break;

			// Just for example, we have the workers let other workers know when they are half done.
			if (counter == num / 2) {
				int tmp_rand = 1 + rand() % 8;
				int result = master_control_block->ipc->message_send(
					master_control_block->ipc->compose_message(tcb, tmp_rand, message_lists[rand() % 18])
				);

				result == 1
					? master_control_block->ui->write_refresh(args->id,
						"\n Message sent\n to Thread #" + std::to_string(tmp_rand) + "\n\n")
					:	master_control_block->ui->write_refresh(args->id,
						"\n Message failed\n to send.\n\n");
			}

			master_control_block->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
			master_control_block->ui->write_refresh(LOG_WINDOW, " Thread #" + std::to_string(args->id) 
				+ " is running #" + std::to_string(counter) + "\n");

			usleep(100000);
		}

		master_control_block->tcb_semaphore->signal();

		sleep(1);
	} while (counter != num);

	master_control_block->ui->write_refresh(args->id, "\n Thread #"
		+ std::to_string(args->id) + "\n has ended.\n");
	master_control_block->ui->write_refresh(LOG_WINDOW, " Thread #"
		+ std::to_string(args->id) + " has ended.\n");

	master_control_block->scheduler->set_state(tcb, DEAD);
	return NULL;
}