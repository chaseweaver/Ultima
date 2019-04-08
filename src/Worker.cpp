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
