#ifndef UFS_H
#define UFS_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <pthread.h>

using namespace std::chrono;

class UFS {
  private:
  struct INODE {
    char filename[8];
    pthread_t owner;
    int starting_block;
    int size;
    char permission[4];
    unsigned int blocks[4];

    int current_read;
    int current_write;

    milliseconds creation_time;
    milliseconds last_modified_time;

    int unique_file_handle;
  };

  std::string fs_name;
  int fs_block_size;
  int fs_number_of_blocks;
  int next_file_handle;
  int next_unique_file_handle;
  char initilization_char;

  Queue< INODE* > nodes;
  char* file_system;

  public:
  UFS(std::string, int, int, char);
  void format();

  int open(int, char[8], char);
  int close(int, int);
  int read_char(int, char*);
  int write_char(int, char);
  int write_char(int, std::string);
  int next_handle();
  int next_unique_handle();

  int create_file(char[8], int, char[4]);
  int delete_file(int, std::string);
  int change_permission(int, std::string, char);
  void dir();
  void dir(int);
  void dump();

  void init_inodes();
  INODE* return_inode(int);
};

#endif