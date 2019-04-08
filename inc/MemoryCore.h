#ifndef MEMORY_CORE_H
#define MEMORY_CORE_H

#ifdef _MSC_VER
#pragma once
#endif

#include <iostream>

class MemoryCore {
  private:
  char* memory;
  const int block_size;
  int memory_capacity;
  int null_character;
  int free_capacity;

  public:
  MemoryCore(const unsigned int, const unsigned int, char);
  ~MemoryCore();
  void write(const unsigned int, char);
  void write_free(const unsigned int, char);
  void read(const unsigned int, char&);
  std::string memory_dump(const unsigned int, const unsigned int);
  std::string memory_dump();
  int memory_blocks();
  int memory_size();
  int memory_left();
};

#endif