#include "../inc/IPC.h"

/*
 * IPC::IPC()
 * Default constructor.
 */
IPC::IPC(MASTER_CONTROL_BLOCK* mcb, int number_of_threads, int max_message_box_size)
  : mcb(mcb), message_box_size(max_message_box_size) {
  for (int i = 1; i <= number_of_threads; i++)
    message_box[i] = *(new Queue< MESSAGE_TYPE* >);
}

/*
 * IPC::~IPC()
 * Default deconstructor.
 */
IPC::~IPC() {}

/*
 * IPC::message_send(MESSAGE_TYPE*)
 * Sends a message to another thread via the IPC.
 * Returns -1 if failed, returns 1 if successful.
 */
int IPC::message_send(MESSAGE_TYPE* message) {
  mcb->ipc_sema->wait();
  std::map< int, Queue< MESSAGE_TYPE* > >::iterator item =
    message_box.find(message->destination_task_id);
  if (item != message_box.end()) {
    if (item->second.size() >= message_box_size) {
      mcb->ipc_sema->signal();
      return -1;
    } else {
      item->second.enqueue(message);

      mcb->ipc_sema->signal();
      return 1;
    }
  } else {
    mcb->ipc_sema->signal();
    return -1;
  }
}

/*
 * IPC::message_receive(int, std::string&)
 * Sets a pointer equal to a message if one exists within the message box.
 * Returns -1 if failed, returns 1 if successful.
 */
int IPC::message_receive(int task_id, std::string& message) {
  mcb->ipc_sema->wait();
  std::map< int, Queue< MESSAGE_TYPE* > >::iterator item = message_box.find(task_id);
  if (item != message_box.end()) {
    if (item->second.size() <= 0) {
      mcb->ipc_sema->signal();
      return -1;
    } else {
      message = item->second.dequeue()->msg;
      mcb->ipc_sema->signal();
      return 1;
    }
  } else {
    mcb->ipc_sema->signal();
    return -1;
  }
}

/*
 * IPC::message_box_count()
 * Returns the amount of message boxes.
 */
int IPC::message_box_count() { return message_box.size(); }

/*
 * IPC::message_count()
 * Returns the amount of messages inside of all message boxes.
 */
int IPC::message_count() {
  int size = 0;

  mcb->ipc_sema->wait();
  std::map< int, Queue< MESSAGE_TYPE* > >::const_iterator item;
  for (item = message_box.begin(); item != message_box.end(); item++)
    size += item->second.size();

  mcb->ipc_sema->signal();
  return size;
}

/*
 * IPC::message_count(int)
 * Returns the amount of messages inside of a Task's message box.
 * Returns -1 if the task is not found.
 */
int IPC::message_count(int task_id) {
  int size = 0;

  mcb->ipc_sema->wait();
  std::map< int, Queue< MESSAGE_TYPE* > >::const_iterator item =
    message_box.find(task_id);
  if (item != message_box.end())
    size = item->second.size();
  else
    size = -1;

  mcb->ipc_sema->signal();
  return size;
}

/*
 * IPC::delete_all_messages(int)
 * Deletes all messages in a given Task's message box.
 */
void IPC::delete_all_messages(int task_id) {
  mcb->ipc_sema->wait();
  std::map< int, Queue< MESSAGE_TYPE* > >::iterator item = message_box.find(task_id);
  if (item != message_box.end())
    while (!item->second.empty()) item->second.dequeue();

  mcb->ipc_sema->signal();
}

/*
 * IPC::fetch_message_box_list()
 * Fetches the message box list.
 */
std::string IPC::fetch_message_box_list() {
  if (message_count() <= 0)
    return "\n There are currently no messages in any message box.\n";

  std::string timestamp = "Timestamp";
  std::string message_size = "Size";
  std::string destination_task_id = "To";
  std::string source_task_id = "From";
  std::string message = "Content";

  pad(timestamp, 14, ' ');
  pad(message_size, 6, ' ');
  pad(destination_task_id, 8, ' ');
  pad(source_task_id, 8, ' ');

  std::string header = "\n " + timestamp + "| " + message_size + "| " +
    destination_task_id + "| " + source_task_id + "| " + message + "\n";

  mcb->ipc_sema->wait();

  std::string content = " ";
  std::map< int, Queue< MESSAGE_TYPE* > >::iterator item;
  for (item = message_box.begin(); item != message_box.end(); item++) {

    if (!item->second.empty()) {
      Queue< MESSAGE_TYPE* >* tmp = new Queue< MESSAGE_TYPE* >(item->second);

      while (!tmp->empty()) {
        MESSAGE_TYPE* tmp_msg = tmp->dequeue();

        std::string timestamp_ = std::to_string(tmp_msg->ms.count());
        std::string message_size_ = std::to_string(tmp_msg->message_size);
        std::string destination_task_id_ =
          "T-ID #" + std::to_string(tmp_msg->destination_task_id);
        std::string source_task_id_ = "T-ID #" + std::to_string(tmp_msg->source_task_id);

        pad(timestamp_, 14, ' ');
        pad(message_size_, 6, ' ');
        pad(destination_task_id_, 8, ' ');
        pad(source_task_id_, 8, ' ');

        content += timestamp_ + "| " + message_size_ + "| " + destination_task_id_ +
          "| " + source_task_id_ + "| " + tmp_msg->msg + "\n";
      }
    }
  }

  mcb->ipc_sema->signal();
  return header + content;
}

/*
 * IPC::fetch_message_box_list()
 * Fetches the message box list.
 */
std::string IPC::fetch_message_box_list(int thread_id) {
  if (message_count(thread_id) <= 0)
    return "\n There are currently no messages in Thread #" + std::to_string(thread_id) +
      "'s inbox.\n";

  std::string timestamp = "Timestamp";
  std::string message_size = "Size";
  std::string source_task_id = "From";
  std::string message = "Content";

  pad(timestamp, 14, ' ');
  pad(message_size, 6, ' ');
  pad(source_task_id, 8, ' ');

  std::string header = "\n Thread #" + std::to_string(thread_id) + "'s Inbox\n\n " +
    timestamp + "| " + message_size + "| " + source_task_id + "| " + message + "\n";

  mcb->ipc_sema->wait();

  std::string content = " ";
  std::map< int, Queue< MESSAGE_TYPE* > >::iterator item;
  for (item = message_box.begin(); item != message_box.end(); item++) {

    if (item->first == thread_id && !item->second.empty()) {
      Queue< MESSAGE_TYPE* >* tmp = new Queue< MESSAGE_TYPE* >(item->second);

      while (!tmp->empty()) {
        MESSAGE_TYPE* tmp_msg = tmp->dequeue();

        std::string timestamp_ = std::to_string(tmp_msg->ms.count());
        std::string message_size_ = std::to_string(tmp_msg->message_size);
        std::string source_task_id_ = "T-ID #" + std::to_string(tmp_msg->source_task_id);

        pad(timestamp_, 14, ' ');
        pad(message_size_, 6, ' ');
        pad(source_task_id_, 8, ' ');

        content += timestamp_ + "| " + message_size_ + "| " + source_task_id_ + "| " +
          tmp_msg->msg + "\n";
      }
    }
  }

  mcb->ipc_sema->signal();

  if (content.length() == 0)
    return "\n There are currently no messages in Thread #" + std::to_string(thread_id) +
      "'s inbox.\n";
  else
    return header + content;
}

/*
 * IPC::compose_message(TASK_CONTROL_BLOCK*, int, std::string)
 * Creates and returns a pointer to a new MESSAGE_TYPE.
 */
IPC::MESSAGE_TYPE* IPC::compose_message(TASK_CONTROL_BLOCK* tcb,
  int destination_task_id,
  std::string message) {
  MESSAGE_TYPE* message_type = new MESSAGE_TYPE;
  message_type->source_task_id = tcb->task_id;
  message_type->destination_task_id = destination_task_id;
  message_type->msg = message;
  message_type->message_size = message.size();
  return message_type;
}