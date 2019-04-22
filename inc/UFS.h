#ifndef UFS_H
#define UFS_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Pad.h"
#include "Queue.h"
#include "Semaphore.h"
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
    std::string status;

    int current_read;
    int current_write;
    int file_id;

    milliseconds creation_time;
    milliseconds last_modified_time;
  };

  MASTER_CONTROL_BLOCK* mcb;
  Semaphore* ufs_sema;

  std::string fs_name;
  int fs_block_size;
  int fs_number_of_blocks;

  int next_file_handle;
  int next_unique_file_handle;
  char initilization_char;

  Queue< INODE* > nodes;
  char* file_system;
  INODE* return_inode(int);
  INODE* return_nonactive_inode(int);
  std::string build_inode(INODE*);
  void init_inodes();
  void write_inodes();
  int set_status(int, std::string);
  int set_read(int);
  int set_write(int);
  int next_handle();
  int next_unique_f_handle();
  bool enough_inodes_available(int);
  bool enough_registered_inodes(int);

  public:
  UFS(MASTER_CONTROL_BLOCK*, std::string, int, int, char);
  ~UFS();
  void format();
  int open(int, std::string, char);
  int close(int);
  int read_char(int, char*);
  int write_char(int, char);
  int write_string(int, std::string);
  int amount_of_inodes();
  int create_file(const std::string, int, const char[4]);
  int delete_file(int, std::string);
  int change_permission(int, std::string, const char[4]);
  std::string dir();
  std::string disk_contents();
  std::string inode_contents();
};

#endif