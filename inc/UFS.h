#ifndef UFS_H
#define UFS_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include <iostream>

class UFS {
  private:

  struct INODE {
    char filename[8];
    int owner_task_id;
    int starting_block;
    int size;
    char permission[4];
    unsigned int blocks[4];

    time_t creation_time;
    time_t last_modified_time;
  };

  std::string fs_name;
  int fs_block_size;
  int fs_number_of_blocks;
  int next_file_handle;
  char initilization_char;

  char* file_system;

  public:
  UFS(std::string, int, int, char);
  void format();

  int open(int, int, std::string, char);
  int close(int, int);
  int read_char(int, int, char*);
  int write_char(int, int, char);

  int create_file(int, std::string, int, char);
  int delete_file(int, std::string);
  int change_permission(int, std::string, char);
  void dir();
  void dir(int);
  void dump();
};

#endif