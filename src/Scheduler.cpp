#include "../inc/Scheduler.h"

/*
 * Scheduler::Scheduler(MASTER_CONTROL_BLOCK*)
 * Default constructor.
 */
Scheduler::Scheduler(MASTER_CONTROL_BLOCK* mcb) : mcb(mcb) {
  assert(!pthread_create(&scheduler_thread, NULL, start_scheduler, this));
}

/*
 * Scheduler::~Scheduler()
 * Default deconstructor.
 */
Scheduler::~Scheduler() {}

/*
 * Scheduler::yield(int, int)
 * Alternative to pthread_yield() since that is not cross-platform.
 */
void Scheduler::yield(int min, int max) { sleep(min + rand() % max); }

/*
 * Scheduler::scheduler()
 * Function to handle state switching.
 */
void Scheduler::scheduler() {
  srand(time(NULL));
  do {
    if (!task_list.empty()) {
      TASK_CONTROL_BLOCK* tcb;
      task_list.front(tcb);

      switch (tcb->task_state) {
        case DEAD:
          mcb->ui->write_refresh(tcb->task_id, " \n Garbage\n Collected.\n");
          mcb->ui->write_refresh(LOG_WINDOW,
            " Garbage Collected: Thread #" + std::to_string(tcb->task_id) + "\n");
          task_list.dequeue();
          delete (tcb);
          --number_of_workers;
          break;

        case IDLE:
          yield(2, 5);
          set_state(tcb, READY);
          task_list.move_to_back();
          break;

        case BLOCKED:
          yield(2, 5);
          set_state(tcb, READY);
          task_list.move_to_back();
          break;

        case READY:
          yield(2, 5);
          set_state(tcb, RUNNING);
          task_list.move_to_back();
          break;

        case RUNNING:
          yield(2, 5);
          set_state(tcb, READY);
          task_list.move_to_back();
          break;
      }
    }
  } while (true);
}

/*
 * Scheduler::create_new_task(std::string, void* (void*), ARGUMENTS*)
 * Creates a new TASK_CONTROL_BLOCK and spawns a new child function in a new thread.
 */
void Scheduler::create_new_task(std::string task_name,
  void* worker(void*),
  ARGUMENTS* task_arguments) {
  TASK_CONTROL_BLOCK* tcb = new TASK_CONTROL_BLOCK;
  tcb->task_id = ++number_of_workers;
  tcb->task_state = task_list.empty() ? RUNNING : READY;
  tcb->task_name = task_name;
  tcb->task_thread = *(new pthread_t);
  task_arguments->task_control_block = tcb;
  tcb->task_arguments = task_arguments;

  mcb->sch_sema->wait();
  task_list.enqueue(tcb);
  mcb->sch_sema->signal();

  assert(!pthread_create(&tcb->task_thread, NULL, worker, tcb->task_arguments));
}

/*
 * Scheduler::task_list_size()
 * Returns the task_list size.
 */
int Scheduler::task_list_size() { return task_list.size(); }

/*
 * Scheduler::fetch_log()
 * Fetches contents of scheduler logs.
 */
std::string Scheduler::fetch_log() {
  if (task_list.empty()) return "\n There are no logs available.";

  std::string title = "\n Scheduler Log\n\n ";
  std::string pbreak = " ";
  std::string task_name = "Thread Name";
  std::string task_id = "Thread ID";
  std::string task_state = "Thread State";

  pad(task_name, 15, ' ');
  pad(pbreak, 44, '-');
  pad(task_id, 12, ' ');
  pad(task_state, 13, ' ');

  std::string header =
    title + task_name + "| " + task_id + "| " + task_state + "\n" + pbreak + "\n";
  mcb->sch_sema->wait();

  std::string content = "";
  if (!task_list.empty()) {
    Queue< TASK_CONTROL_BLOCK* >* tmp = new Queue< TASK_CONTROL_BLOCK* >(task_list);
    do {
      TASK_CONTROL_BLOCK* tcb;
      tmp->dequeue(tcb);

      std::string task_name_ = tcb->task_name;
      std::string task_id_ = "Thread #" + std::to_string(tcb->task_id);
      std::string task_state_;

      switch (tcb->task_state) {
        case DEAD: task_state_ = "DEAD"; break;
        case IDLE: task_state_ = "IDLE"; break;
        case BLOCKED: task_state_ = "BLOCKED"; break;
        case READY: task_state_ = "READY"; break;
        case RUNNING: task_state_ = "RUNNING"; break;
      }

      pad(task_name_, 14, ' ');
      pad(task_id_, 11, ' ');

      content += " " + task_name_ + " | " + task_id_ + " | " + task_state_ + "\n";
    } while (!tmp->empty());
  }

  mcb->sch_sema->signal();
  return header + content;
}

/*
 * Scheduler::set_state(TASK_CONTROL_BLOCK*, int)
 * Changes task state in the list and logs to STATE WINDOW.
 */
void Scheduler::set_state(TASK_CONTROL_BLOCK* tcb, int state) {
  if (tcb->task_state == state || tcb->task_state == DEAD) return;

  std::string str_old = "";
  switch (tcb->task_state) {
    case DEAD: str_old = "DEAD"; break;
    case IDLE: str_old = "IDLE"; break;
    case BLOCKED: str_old = "BLOCKED"; break;
    case READY: str_old = "READY"; break;
    case RUNNING: str_old = "RUNNING"; break;
  }

  tcb->task_state = state;

  std::string str_new = "";
  switch (state) {
    case DEAD: str_new = "DEAD"; break;
    case IDLE: str_new = "IDLE"; break;
    case BLOCKED: str_new = "BLOCKED"; break;
    case READY: str_new = "READY"; break;
    case RUNNING: str_new = "RUNNING"; break;
  }

  mcb->ui->write_refresh(STATE_WINDOW,
    " Thread #" + std::to_string(tcb->task_id) + " " + str_old + " -> " + str_new + "\n");
  mcb->logger->add_log(tcb->task_id, tcb->task_name, tcb->task_state);
}

/*
 * Scheduler::set_state(int*, int)
 * Changes task state in the list and logs to STATE WINDOW.
 */
void Scheduler::set_state(int task_id, int state) {
  TASK_CONTROL_BLOCK* tcb = get_task_control_block(task_id);

  if (tcb->task_id == state) return;

  std::string str_old = "";
  switch (tcb->task_state) {
    case DEAD: str_old = "DEAD"; break;
    case IDLE: str_old = "IDLE"; break;
    case BLOCKED: str_old = "BLOCKED"; break;
    case READY: str_old = "READY"; break;
    case RUNNING: str_old = "RUNNING"; break;
  }

  tcb->task_state = state;

  std::string str_new = "";
  switch (state) {
    case DEAD: str_new = "DEAD"; break;
    case IDLE: str_new = "IDLE"; break;
    case BLOCKED: str_new = "BLOCKED"; break;
    case READY: str_new = "READY"; break;
    case RUNNING: str_new = "RUNNING"; break;
  }

  mcb->ui->write_refresh(STATE_WINDOW,
    " Thread #" + std::to_string(tcb->task_id) + " " + str_old + " -> " + str_new + "\n");
  mcb->logger->add_log(tcb->task_id, tcb->task_name, tcb->task_state);
}

/*
 * Scheduler::start_scheduler(void*)
 * Starts the scheduler loop in a new thread.
 */
void* Scheduler::start_scheduler(void* p) {
  static_cast< Scheduler* >(p)->scheduler();
  return NULL;
}

/*
 * Scheduler::create_arguments(int, int)
 * Creates a new ARGUMENTS struct.
 */
ARGUMENTS* Scheduler::create_arguments(int id, int thread_results) {
  ARGUMENTS* args = new ARGUMENTS;
  args->id = id;
  args->thread_results = thread_results;
  return args;
}

/*
 * Scheduler::create_arguments(int, int, TASK_CONTROL_BLOCK*)
 * Creates a new ARGUMENTS struct.
 */
ARGUMENTS* Scheduler::create_arguments(int id,
  int thread_results,
  TASK_CONTROL_BLOCK* task_control_block) {
  ARGUMENTS* args = new ARGUMENTS;
  args->id = id;
  args->thread_results = thread_results;
  args->task_control_block = task_control_block;
  return args;
}

/*
 * Scheduler::get_task_control_block(int)
 * Returns a pointer to a thread's TASK_CONTROL_BLOCK given an ID.
 */
TASK_CONTROL_BLOCK* Scheduler::get_task_control_block(int tid) {
  Queue< TASK_CONTROL_BLOCK* >* tmp = new Queue< TASK_CONTROL_BLOCK* >(task_list);

  do {
    TASK_CONTROL_BLOCK* tcb;
    tmp->dequeue(tcb);
    if (tcb->task_id == tid) return tcb;
  } while (!tmp->empty());
  return NULL;
}