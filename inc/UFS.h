#ifndef UFS_H
#define UFS_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Pad.h"
#include "Queue.h"
#include <bitset>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string.h>

using namespace std::chrono;

class UFS {
  private:
  struct INODE {
    std::string filename;
    pthread_t owner;
    int block_id;
    int size;
    char permission[4];
    bool active;

    int current_read;
    int current_write;
    int id;

    milliseconds creation_time;
    milliseconds last_modified_time;
  };

  MASTER_CONTROL_BLOCK* mcb;

  std::string fs_name;
  int fs_block_size;
  int fs_number_of_blocks;

  int next_file_handle;
  int next_unique_file_handle;
  char initilization_char;

  Queue< INODE* > nodes;
  char* file_system;

  public:
  UFS(MASTER_CONTROL_BLOCK*, std::string, int, int, char);
  ~UFS();
  void format();

  int open(int, std::string, char);
  int close(int, int);
  int read_char(int, char*);
  int write_char(int, char);
  int write_string(int, std::string);
  int next_handle();
  int amount_of_inodes();
  int next_unique_handle();

  int create_file(const std::string, int, char[4]);
  int delete_file(int, std::string);
  int change_permission(int, std::string, char);

  void init_inodes();
  void write_inodes();
  std::string deconstruct_inode(INODE*);
  std::string read_inodes();
  std::string char_to_binary(unsigned char);
  std::string dir();
  std::string disk_contents();
  INODE* return_inode(int);
  INODE* construct_inode(std::string);
};

#endif