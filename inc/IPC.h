#ifndef IPC_H
#define IPC_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Pad.h"
#include "Queue.h"
#include "Struct.h"
#include <chrono>
#include <iostream>
#include <map>

using namespace std::chrono;

class IPC {
  private:
  struct MESSAGE_TYPE {
    int source_task_id;
    int destination_task_id;
    int message_size;
    std::string msg;
    const milliseconds ms =
      duration_cast< milliseconds >(system_clock::now().time_since_epoch());
  };

  MASTER_CONTROL_BLOCK* mcb;
  std::map< int, Queue< MESSAGE_TYPE* > > message_box;
  int message_box_size;

  public:
  IPC(MASTER_CONTROL_BLOCK*, int, int);
  ~IPC();
  int message_send(MESSAGE_TYPE*);
  int message_receive(int, std::string&);
  int message_box_count();
  int message_count();
  int message_count(int);
  void delete_all_messages(int);
  std::string fetch_message_box_list();
  std::string fetch_message_box_list(int);
  MESSAGE_TYPE* compose_message(TASK_CONTROL_BLOCK*, int, std::string);
};

#endif