#ifndef I_NODE_H
#define I_NODE_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

class I_Node {
  char filename[8];
  int owner_task_id;
  int starting_block;
  int size;
  char permission[4];
  unsigned int blocks[4];
};

#endif