#ifndef MASTER_CONTROL_BLOCK_H
#define MASTER_CONTROL_BLOCK_H

#ifdef _MSC_VER
#pragma once
#endif

#define HEADING_WINDOW -1
#define LOG_WINDOW -2
#define STATE_WINDOW -3
#define CONSOLE_WINDOW -4
#define OUTPUT_WINDOW -5
#define MAILBOX_WINDOW -6

class Menu;
class Scheduler;
class Semaphore;
class Logger;
class UI;

#include "Scheduler.h"
#include "UI.h"
#include "Menu.h"
#include "Semaphore.h"
#include "Logger.h"
#include "Queue.h"

struct MASTER_CONTROL_BLOCK {
	Scheduler* scheduler;
	UI* ui;
	Menu* menu;
	Semaphore* ui_semaphore;
	Semaphore* scheduler_semaphore;
	Semaphore* logger_semaphore;
	Semaphore* tcb_semaphore;
	Logger* logger;
};

#endif