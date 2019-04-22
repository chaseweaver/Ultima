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
  int create, open, write, close;

  std::string message_lists[20] = {
    "A woman gets on a bus with her baby. The bus driver says: 'Ugh, that's the ugliest "
    "baby I've ever seen!' The woman walks to the rear of the bus and sits down, fuming. "
    "She says to a man next to her: 'The driver just insulted me!' The man says: 'You go "
    "up there and tell him off. Go on, I'll hold your monkey for you.'",
    "What does Pac-Man eat with his chips? Guacawakamole!",
    "Whenever the cashier at the grocery store asks my dad if he would like the milk in "
    "a bag he replies, 'No, just leave it in the carton!'",
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

      std::string msg = message_lists[rand() % 20];

      if (counter == num / 4) {
        tracker = mcb->mem_man->allocate(msg.length());
        mcb->mem_man->write(tracker, msg);
        mcb->ui->write_refresh(
          LOG_WINDOW, " Alloc. memory for Thread #" + std::to_string(args->id) + "\n");
      }

      if (counter == num / 3) {
        std::string name = "Thread #" + std::to_string(args->id) + ".txt";

        mcb->ui->write_refresh(
          LOG_WINDOW, " Creating file for Thread #" + std::to_string(args->id) + "\n");

        char const* perm = "rw--";
        create = mcb->ufs->create_file(name, msg.length() + 1, perm);
        open = mcb->ufs->open(create, name, 'w');
        if (open != -1) {
          mcb->ui->write_refresh(
            LOG_WINDOW, " Opening file for Thread #" + std::to_string(args->id) + "\n");
          write = mcb->ufs->write_string(create, msg);
        } else {
          mcb->ui->write_refresh(
            LOG_WINDOW, " Failed to create file for Thread #" + std::to_string(args->id));
        }
      }

      if (counter == (num / 3) + 5) {
        mcb->ui->write_refresh(
          LOG_WINDOW, " Closing file for Thread #" + std::to_string(args->id) + "\n");
        close = mcb->ufs->close(open);
      }

      if (counter == num - 20) {
        char const* perm = "rwrw";
        mcb->ufs->change_permission(
          open, "Thread #" + std::to_string(args->id) + ".txt", perm);
        mcb->ui->write_refresh(
          LOG_WINDOW, " Changing perms. for Thread #" + std::to_string(args->id) + "\n");
      }

      if (counter == num / 2) {
        int tmp_rand = 1 + rand() % 8;
        int result =
          mcb->ipc->message_send(mcb->ipc->compose_message(tcb, tmp_rand, msg));

        result == 1 ? mcb->ui->write_refresh(LOG_WINDOW,
                        " Msg sent: Thread #" + std::to_string(args->id) +
                          " -> Thread #" + std::to_string(tmp_rand) + "\n")
                    : mcb->ui->write_refresh(LOG_WINDOW,
                        " Msg failed to send: Thread #" + std::to_string(args->id) +
                          " -> Thread #" + std::to_string(tmp_rand) + "\n");
      }
      mcb->ui->write_refresh(args->id, " Running #" + std::to_string(++counter) + "\n");
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

  mcb->ufs->delete_file(open, "Thread #" + std::to_string(args->id));
  mcb->scheduler->set_state(tcb, DEAD);
  return NULL;
}
