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
class UI;

#include "Scheduler.h"
#include "UI.h"
#include "Menu.h"
// #include "Semaphore.h"
#include "Logger.h"

struct MASTER_CONTROL_BLOCK {
	Scheduler* scheduler;
	UI* ui;
	Menu* menu;
	// Semaphore* scheduler_semaphore = new Semaphore("UI Handler", 1);
	// Semaphore* icp_semaphore = new Semaphore("ICP Handler", 1);
	Logger* logger = new Logger(12);
};

#endif