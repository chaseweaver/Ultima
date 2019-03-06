#ifndef MASTER_CONTROL_BLOCK_H
#define MASTER_CONTROL_BLOCK_H

#ifdef _MSC_VER
#pragma once
#endif

#include "Scheduler.h"
#include "UI.h"
#include "Menu.h"
#include "Semaphore.h"
#include "Logger.h"

struct MASTER_CONTROL_BLOCK {
	Scheduler* scheduler = new Scheduler();
	UI* ui = new UI();
	Menu* menu;
	Semaphore* scheduler_semaphore = new Semaphore("UI Handler", 1);
	Semaphore* icp_semaphore = new Semaphore("ICP Handler", 1);
	Logger* logger = new Logger(12);
}* master_control_block;

#endif