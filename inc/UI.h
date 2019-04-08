#ifndef UI_H
#define UI_H

#ifdef _MSC_VER
#pragma once
#endif

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include "Scheduler.h"
#include "Struct.h"
#include <iostream>
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

class UI {
  private:
  struct WINDOW_OBJECT {
    std::string window_title;
    int window_id;
    int window_width;
    int window_height;
    int window_x;
    int window_y;
    WINDOW* window;
  };

  MASTER_CONTROL_BLOCK* mcb;
  Queue< WINDOW_OBJECT* >* window_object;

  WINDOW* fetch_window(int);
  bool write_window(WINDOW*, std::string);
  bool write_window(WINDOW*, int, int, std::string);
  bool write_window_refresh(WINDOW*, std::string);
  bool write_window_refresh(WINDOW*, int, int, std::string);

  public:
  UI(MASTER_CONTROL_BLOCK*);
  ~UI();
  WINDOW* create_window_spawn(std::string, int, int, int, int, int, int, int);
  WINDOW* create_window(std::string, int, int, int, int, int, int, int);
  WINDOW* create_window_spawn(std::string, int, int, int, int, int);
  WINDOW* create_window(std::string, int, int, int, int, int);
  WINDOW* create_window_spawn(int, int, int, int, int);
  WINDOW* create_window(int, int, int, int, int);
  WINDOW* create_window_lock_spawn(std::string, int, int, int, int, int, int, int);
  WINDOW* create_window_lock(std::string, int, int, int, int, int, int, int);
  void write(int, int, int, std::string, TASK_CONTROL_BLOCK*);
  void write(int, int, int, std::string);
  void write(int, std::string, TASK_CONTROL_BLOCK*);
  void write(int, std::string);
  void write_refresh(int, int, int, std::string, TASK_CONTROL_BLOCK*);
  void write_refresh(int, int, int, std::string);
  void write_refresh(int, std::string, TASK_CONTROL_BLOCK*);
  void write_refresh(int, std::string);
  void clear_window(WINDOW*);
  void clear_window(int);
  int get_window_amount();
};

#endif