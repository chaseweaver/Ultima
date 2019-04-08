#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "MemoryCore.h"
#include "Queue.h"
#include "Semaphore.h"
#include <iostream>
#include <pthread.h>

#define HOLE 0
#define PROCESS 1

class MemoryManager {
  private:
  struct MEMORY_NODE {
    pthread_t owner;
    int base;
    int limit;
    int handle;
    int status;
    int current_read;
    int current_write;
  };

  MemoryCore* mem_core;
  const int block_size;
  int memory_handle;

  Queue< MEMORY_NODE* > memory_list;
  int next_handle();
  MEMORY_NODE* find_hole(int);
  void memory_cleanup();

  MASTER_CONTROL_BLOCK* mcb;
  Semaphore* mem_sema;

  public:
  MemoryManager(const unsigned int, const unsigned int, char);
  ~MemoryManager();

  int allocate(const unsigned int);
  int read(int, char&);
  int read(int, int, std::string&);
  int write(int, char);
  int write(int, int, char);
  int write(int, std::string);
  int write(int, int, std::string);
  int write(int, int, int, std::string);
  int memory_left();
  int memory_largest();
  int memory_smallest();

  void free(int);
  void coalesce(int);

  std::string memory_dump(const unsigned int, const unsigned int);
  std::string memory_dump();
  std::string memory_dump_mem();
};

#endif
