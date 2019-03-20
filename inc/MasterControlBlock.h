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
#define MAILBOX_WINDOW -6
#define INPUT_WINDOW -7

class Menu;
class Scheduler;
class Semaphore;
class Logger;
class UI;
class IPC;
class Worker;
class MemoryManager;

#include "Scheduler.h"
#include "UI.h"
#include "Menu.h"
#include "Semaphore.h"
#include "Logger.h"
#include "Queue.h"
#include "IPC.h"
#include "Worker.h"
#include "MemoryManager.h"

struct MASTER_CONTROL_BLOCK {
	Scheduler* scheduler;
	UI* ui;
	Menu* menu;
	IPC* ipc;
	Worker* worker;
	Logger* logger;
	MemoryManager* memory_manager;
	Semaphore* ui_semaphore;
	Semaphore* scheduler_semaphore;
	Semaphore* logger_semaphore;
	Semaphore* tcb_semaphore;
	Semaphore* ipc_semaphore;
};

#endif