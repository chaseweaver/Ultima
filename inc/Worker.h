#ifndef WORKER_H
#define WORKER_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Scheduler.h"
#include "Semaphore.h"
#include "Struct.h"
#include <iostream>

class Worker {
  private:
  void* worker_function(void*);
  MASTER_CONTROL_BLOCK* mcb;

  public:
  Worker(MASTER_CONTROL_BLOCK*);
  ~Worker();
  static void* start_worker_function(void*);
};

#endif
