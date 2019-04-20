#ifndef MENU_H
#define MENU_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Struct.h"
#include <ncurses.h>

class Menu {
  private:
  WINDOW* menu_window;
  bool enabled = false;
  pthread_t menu_thread;

  MASTER_CONTROL_BLOCK* mcb;

  void menu();
  static void* start_menu(void*);
  void print_log(int);
  void print_scheduler_logs(int);
  void print_tcb_sema(int);
  void print_message_box_list(int);
  void get_thread_message_box(int, int);
  void print_thread_menu(int);
  void print_thread_inbox_menu(int);
  void print_ufs_menu(int);
  void print_memory_mgmt_menu(int);

  public:
  Menu(MASTER_CONTROL_BLOCK*, WINDOW*);
  ~Menu();
  void start();
  void stop();
  void wait();
  void print_menu(int);
};

#endif