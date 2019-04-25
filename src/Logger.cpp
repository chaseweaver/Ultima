#include "../inc/Logger.h"

/*
 * Logger::Logger(int)
 * Default constructor.
 */
Logger::Logger(int max_number_of_logs_kept)
  : MAX_NUMBER_OF_LOGS_KEPT(max_number_of_logs_kept) {}

/*
 * Logger::~Logger()
 * Default deconstructor.
 */
Logger::~Logger() {}

/*
 * Logger::add_log(int, std::string, int)
 * Adds a log to the queue.
 */
void Logger::add_log(int task_id, std::string task_name, int task_state) {
  if (log_data.size() >= MAX_NUMBER_OF_LOGS_KEPT) log_data.dequeue();

  LOG_DATA* log = new LOG_DATA;
  log->task_id = task_id;
  log->task_name = task_name;
  log->task_state = task_state;
  log_data.enqueue(log);
}

/*
 * Logger::get_max_number_of_logs_kept()
 * Returns the maximum number of logs to be kept.
 */
int Logger::get_max_number_of_logs_kept() { return MAX_NUMBER_OF_LOGS_KEPT; }

/*
 * Logger::set_max_number_of_logs_kept(int)
 * Sets the maximum number of logs to be kept.
 */
void Logger::set_max_number_of_logs_kept(int max_number_of_logs_kepts) {
  MAX_NUMBER_OF_LOGS_KEPT = max_number_of_logs_kepts;
}

/*
 * Logger::fetch_log()
 * Fetches contents of logs based on MAX_NUMBER_OF_LOGS_KEPT.
 */
std::string Logger::fetch_log() {
  if (log_data.empty()) return "\n There are no logs available.";

  Queue< LOG_DATA* >* log_data_ = new Queue< LOG_DATA* >(log_data);

  std::string task_id = "Task ID";
  std::string pbreak = " ";
  std::string task_name = "Task Name";
  std::string task_state = "Task State";
  std::string task_timestamp = "Task Timestamp";

  pad(task_id, 9, ' ');
  pad(pbreak, 55, '-');
  pad(task_name, 13, ' ');
  pad(task_state, 12, ' ');

  std::string header = "\n " + task_id + "| " + task_name + "| " + task_state + "| " +
    task_timestamp + "\n" + pbreak + "\n";
  std::string content = "";
  do {

    LOG_DATA* tmp;
    log_data_->dequeue(tmp);
    std::string task_id_ = "T-ID #" + std::to_string(tmp->task_id);
    std::string task_name_ = tmp->task_name;
    std::string task_timestamp_ = std::to_string(tmp->ms.count());

    std::string task_state_;
    switch (tmp->task_state) {
      case 0: task_state_ = "DEAD"; break;
      case 1: task_state_ = "BLOCKED"; break;
      case 2: task_state_ = "IDLE"; break;
      case 3: task_state_ = "READY"; break;
      case 4: task_state_ = "RUNNING"; break;
    }

    pad(task_id_, 9, ' ');
    pad(task_name_, 13, ' ');
    pad(task_state_, 12, ' ');

    content += " " + task_id_ + "| " + task_name_ + "| " + task_state_ + "| " +
      task_timestamp_ + "\n";
  } while (!log_data_->empty());

  return header + content;
}