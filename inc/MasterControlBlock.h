#ifndef MASTER_CONTROL_BLOCK_H
#define MASTER_CONTROL_BLOCK_H

#ifdef _MSC_VER
#pragma once
#endif

#define HEADING_WINDOW -1
#define LOG_WINDOW -2
#define STATE_WINDOW -3
#define MENU_WINDOW -4
#define OUTPUT_WINDOW -5
#define INPUT_WINDOW -6
#define TESTING_WINDOW -7

class Menu;
class Scheduler;
class Semaphore;
class Logger;
class UI;
class IPC;
class Worker;
class MemoryManager;
class UFS;

#include "IPC.h"
#include "Logger.h"
#include "MemoryManager.h"
#include "Menu.h"
#include "Queue.h"
#include "Scheduler.h"
#include "Semaphore.h"
#include "UFS.h"
#include "UI.h"
#include "Worker.h"

struct MASTER_CONTROL_BLOCK {
  Scheduler* scheduler;
  UI* ui;
  Menu* menu;
  IPC* ipc;
  Worker* worker;
  Logger* logger;
  MemoryManager* mem_man;
  Semaphore* ui_sema;
  Semaphore* sch_sema;
  Semaphore* log_sema;
  Semaphore* tcb_sema;
  Semaphore* ipc_sema;
  UFS* ufs;
};

#endif
