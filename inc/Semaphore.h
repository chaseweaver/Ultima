#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include "Struct.h"
#include <condition_variable>
#include <mutex>
#include <thread>

class Semaphore {
  private:
  std::mutex mtx;
  std::condition_variable cv;
  std::string resource_name;
  int value;

  Queue< TASK_CONTROL_BLOCK* > sema_queue;
  MASTER_CONTROL_BLOCK* mcb;

  public:
  Semaphore(MASTER_CONTROL_BLOCK*, std::string, int);
  ~Semaphore();
  void wait();
  void wait(TASK_CONTROL_BLOCK*);
  void signal();
  std::string fetch_log();
};

#endif