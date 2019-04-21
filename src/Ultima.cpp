#include "../inc/MasterControlBlock.h"

#include <sstream>

void mcb_init();
void window_init();
void* worker_function(void*);

MASTER_CONTROL_BLOCK* mcb = new MASTER_CONTROL_BLOCK;
int NUMBER_OF_WORKERS = 8;

int main() {

  mcb_init();
  window_init();

  // Spawn child workers
  for (int i = 1; i <= NUMBER_OF_WORKERS; i++) {
    i <= 4
      ? mcb->ui->create_window_lock_spawn(
          " Worker #" + std::to_string(i) + ' ', 4, 0, i, 19, 10, 3 + ((i - 1) * 20), 14)
      : mcb->ui->create_window_lock_spawn(
          " Worker #" + std::to_string(i) + ' ', 4, 0, i, 19, 10, 3 + ((i - 5) * 20), 24);

    // In the future, separate worker functions into their own classes.
    mcb->scheduler->create_new_task("Worker #" + std::to_string(i),
      worker_function,
      mcb->scheduler->create_arguments(i, 0));
  }

  mcb->menu->wait();
  return 0;
}

/*
 * Ultima::mcb_init()
 * Initializes the MASTER_CONTROL_BLOCK objects.
 */
void mcb_init() {
  mcb->ui_sema = new Semaphore(mcb, "UI Handler", 1);
  mcb->sch_sema = new Semaphore(mcb, "Scheduler Handler", 1);
  mcb->log_sema = new Semaphore(mcb, "Logger Handler", 1);
  mcb->tcb_sema = new Semaphore(mcb, "TCB Locker", 1);
  mcb->ipc_sema = new Semaphore(mcb, "IPC Handler", 1);
  mcb->scheduler = new Scheduler(mcb);
  mcb->ui = new UI(mcb);
  mcb->logger = new Logger(32);
  mcb->ipc = new IPC(mcb, NUMBER_OF_WORKERS, 8);
  mcb->worker = new Worker(mcb);
  mcb->mem_man = new MemoryManager(1024, 32, '.');
  mcb->menu = new Menu(
    mcb, mcb->ui->create_window_lock_spawn(" Menu ", 2, 0, MENU_WINDOW, 58, 12, 83, 34));
  mcb->ufs = new UFS(mcb, "root", 16, 128, '_');
}

/*
 * Ultima::window_init()
 * Creates the initial windows.
 */
void window_init() {
  mcb->ui->create_window_spawn(HEADING_WINDOW, 79, 12, 3, 2);
  mcb->ui->write(HEADING_WINDOW, 26, 2, "ULTIMA 2.0 (Spring 2019)");
  mcb->ui->write(HEADING_WINDOW, 18, 3, "The Washington Redskins (Matt + Chase)");
  mcb->ui->write(HEADING_WINDOW, 2, 6, "$ Starting UI handler...");
  mcb->ui->write_refresh(HEADING_WINDOW, 2, 7, "$ Spawning child threads...");

  mcb->ui->create_window_lock_spawn(" Log ", 2, 0, LOG_WINDOW, 39, 12, 3, 34);
  mcb->ui->create_window_lock_spawn(" State ", 2, 0, STATE_WINDOW, 39, 12, 43, 34);
  mcb->ui->create_window_lock_spawn(
    " Output / Mailbox / Memory Core / File System ", 2, 0, OUTPUT_WINDOW, 94, 32, 83, 2);
  mcb->ui->create_window_lock_spawn(" Input ", 2, 0, INPUT_WINDOW, 35, 12, 142, 34);
  mcb->ui->write(INPUT_WINDOW, "\n");
  mcb->menu->print_menu(MENU_WINDOW);
}

/*
 * Ultima::worker_function(void*)
 * Worker function to run in separate threads.
 * It also tells jokes.
 */
void* worker_function(void* arguments) {
  ARGUMENTS* args = (ARGUMENTS*)arguments;
  TASK_CONTROL_BLOCK* tcb = args->task_control_block;
  int& counter = args->thread_results;
  int tracker;

  std::string message_lists[17] = {
    "Did you hear about the restaurant on the moon? Great food, no atmosphere.",
    "What do you call a fake noodle? An Impasta.",
    "How many apples grow on a tree? All of them.",
    "Want to hear a joke about paper? Nevermind it's tearable.",
    "Why did the coffee file a police report? It got mugged.",
    "How does a penguin build its house? Igloos it together.",
    "Dad, did you get a haircut? No I got them all cut.",
    "Why did the scarecrow win an award? Because he was outstanding in his "
    "field.",
    "What do you call an elephant that doesn't matter? An irrelephant!",
    "What do you call cheese that isn't yours? Nacho Cheese.",
    "What did the grape do when he got stepped on? He let out a little wine.",
    "I would avoid the sushi if I was you. It's a little fishy.",
    "What's brown and sticky? A stick.",
    "I thought about going on an all-almond diet. But that's just nuts.",
    "People don't like having to bend over to get their drinks. We really need "
    "to raise the bar.",
    "I don't play soccer because I enjoy the sport. I'm just doing it for "
    "kicks.",
    "Why do you never see elephants hiding in trees? Because they're so good "
    "at it."};

  int num = 1 + rand() % 100;
  do {
    mcb->tcb_sema->wait(tcb);

    while (tcb->task_state == RUNNING) {
      if (counter == num) break;

      std::string msg = message_lists[rand() % 16];

      if (counter == num / 4) {
        tracker = mcb->mem_man->allocate(msg.length());
        mcb->mem_man->write(tracker, msg);
        mcb->ui->write_refresh(LOG_WINDOW,
          " Allocating memory for Thread #" + std::to_string(args->id) + "\n");
      }

      if (counter == num / 3) {
        std::string name = "Thread #" + std::to_string(args->id);
        char cstr[name.size() + 1];
        strcpy(cstr, name.c_str());

        msg = "00000000000000000000000000000000000000000000000000000000000000000000000000"
              "0000000000000000000000000000000000000000000000000000000";

        int create = mcb->ufs->create_file(cstr, msg.length() + 1, "rw--");
        if (mcb->ufs->open(create, "Thread #" + std::to_string(args->id), 'w') == 1)
          int write = mcb->ufs->write_string(create, msg);
      }

      // Just for example, we have the workers let other workers know when they
      // are half done. For a bonus, they tell jokes.
      if (counter == num / 2) {
        int tmp_rand = 1 + rand() % 8;
        int result =
          mcb->ipc->message_send(mcb->ipc->compose_message(tcb, tmp_rand, msg));

        // Did the message fail to send or not?
        result == 1
          ? mcb->ui->write_refresh(args->id,
              "\n Message sent\n to Thread #" + std::to_string(tmp_rand) + "\n\n")
          : mcb->ui->write_refresh(args->id, "\n Message failed\n to send.\n\n");
      }

      mcb->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
      mcb->ui->write_refresh(LOG_WINDOW,
        " Thread #" + std::to_string(args->id) + " is running #" +
          std::to_string(counter) + "\n");

      usleep(100000);
    }

    mcb->tcb_sema->signal();

    sleep(1);
  } while (counter != num);

  mcb->mem_man->free(tracker);

  mcb->ui->write_refresh(
    args->id, "\n Thread #" + std::to_string(args->id) + "\n has ended.\n");
  mcb->ui->write_refresh(
    LOG_WINDOW, " Thread #" + std::to_string(args->id) + " has ended.\n");

  mcb->scheduler->set_state(tcb, DEAD);
  return NULL;
}
