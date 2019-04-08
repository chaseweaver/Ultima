#ifndef LOGGER_H
#define LOGGER_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Pad.h"
#include "Queue.h"
#include <chrono>
#include <iostream>

using namespace std::chrono;

class Logger {
  private:
  struct LOG_DATA {
    int task_id;
    int task_state;
    std::string task_name;
    const milliseconds ms =
      duration_cast< milliseconds >(system_clock::now().time_since_epoch());
  };

  MASTER_CONTROL_BLOCK* mcb;
  Queue< LOG_DATA* > log_data;
  int MAX_NUMBER_OF_LOGS_KEPT;

  public:
  Logger(int);
  ~Logger();
  void add_log(int, std::string, int);
  int get_max_number_of_logs_kept();
  void set_max_number_of_logs_kept(int);
  std::string fetch_log();
};

#endif