#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include "Struct.h"
#include <assert.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define DEAD 0
#define BLOCKED 1
#define IDLE 2
#define READY 3
#define RUNNING 4

class Scheduler {
  private:
  MASTER_CONTROL_BLOCK* mcb;
  Queue< TASK_CONTROL_BLOCK* > task_list;

  int number_of_workers = 0;

  pthread_t scheduler_thread;

  void yield(int, int);
  void scheduler();

  static void* start_scheduler(void*);

  public:
  Scheduler(MASTER_CONTROL_BLOCK*);
  ~Scheduler();
  void create_new_task(std::string, void*(void*), ARGUMENTS*);
  void set_state(TASK_CONTROL_BLOCK*, int);
  void set_state(int, int);
  int task_list_size();

  std::string fetch_log();

  ARGUMENTS* create_arguments(int, int);
  ARGUMENTS* create_arguments(int, int, TASK_CONTROL_BLOCK*);

  TASK_CONTROL_BLOCK* get_task_control_block(int);
};

#endif