#include "../inc/Worker.h"

/*
 * Worker::worker(MASTER_CONTROL_BLOCK*)
 * Default constructor.
 */
Worker::Worker(MASTER_CONTROL_BLOCK* mcb) : mcb(mcb) {}

/*
 * Worker::~worker()
 * Default deconstructor.
 */
Worker::~Worker() {}

/*
 * Worker::start_worker_function(void*)
 * Helper function to start the worker.
 */
void* Worker::start_worker_function(void* p) {
  static_cast< Worker* >(p)->worker_function(p);
  return NULL;
}

/*
 * Worker::worker_function(void*)
 * Worker function to run in separate threads.
 */
void* Worker::worker_function(void* arguments) {
  ARGUMENTS* args = (ARGUMENTS*)arguments;
  TASK_CONTROL_BLOCK* tcb = args->task_control_block;
  int& counter = args->thread_results;
  int tracker;
  int create, open, write, close;

  std::string message_lists[18] = {
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

      std::string msg = message_lists[rand() % 18];

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
          close = mcb->ufs->close(open);
        } else {
          mcb->ui->write_refresh(
            LOG_WINDOW, " Failed to create file for Thread #" + std::to_string(args->id));
        }
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
