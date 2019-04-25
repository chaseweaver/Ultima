#include "../inc/Semaphore.h"

/*
 * Semaphore::Semaphore(std::string, int)
 * Default constructor.
 */
Semaphore::Semaphore(MASTER_CONTROL_BLOCK* mcb, std::string name, int val)
  : mcb(mcb), resource_name(name), value(val) {}

/*
 * Semaphore::~Semaphore()
 * Default deconstructor.
 */
Semaphore::~Semaphore() {}

/*
 * Semaphore::wait()
 * Locks threads depending on state.
 */
void Semaphore::wait() {
  std::unique_lock< std::mutex > lock(mtx);
  while (value == 0) cv.wait(lock);
  --value;
}

/*
 * Semaphore::wait(TASK_CONTROL_BLOCK*)
 * Locks threads depending on state.
 */
void Semaphore::wait(TASK_CONTROL_BLOCK* tcb) {
  std::unique_lock< std::mutex > lock(mtx);
  if (value == 0) sema_queue.enqueue(tcb);
  while (value == 0) {
    if (tcb->task_state != DEAD) mcb->scheduler->set_state(tcb, BLOCKED);
    cv.wait(lock);
  }

  if (tcb->task_state != DEAD) mcb->scheduler->set_state(tcb, RUNNING);
  --value;
}

/*
 * Semaphore::signal()
 * Signals to other threads that one has finished.
 */
void Semaphore::signal() {
  std::unique_lock< std::mutex > lock(mtx);
  ++value;
  if (!sema_queue.empty()) sema_queue.dequeue();
  cv.notify_one();
}

/*
 * Semaphore::fetch_log()
 * Fetches contents of semaphore logs.
 */
std::string Semaphore::fetch_log() {
  std::string header = "\n Semaphore Log\n";
  std::string sema_title = " Title:";
  std::string sema_value_title = " Value:";
  std::string sema_list_title = " Queue:";

  pad(sema_title, 9, ' ');
  pad(sema_value_title, 9, ' ');
  pad(sema_list_title, 9, ' ');

  sema_title += resource_name;
  sema_value_title += std::to_string(value);

  if (!sema_queue.empty()) {
    Queue< TASK_CONTROL_BLOCK* >* tcb = new Queue< TASK_CONTROL_BLOCK* >(sema_queue);
    do {
      TASK_CONTROL_BLOCK* tmp;
      tcb->dequeue(tmp);
      sema_list_title += tcb->size() > 0 ? std::to_string(tmp->task_id) + " -> "
                                         : std::to_string(tmp->task_id);
    } while (!tcb->empty());
  }

  if (sema_list_title.length() == 9)
    sema_list_title += "There are no tasks in the queue.";

  return header + "\n" + sema_title + "\n" + sema_value_title + "\n" + sema_list_title;
}