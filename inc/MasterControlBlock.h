#ifndef MASTER_CONTROL_BLOCK_H
#define MASTER_CONTROL_BLOCK_H

#ifdef _MSC_VER
#pragma once
#endif

#include "Scheduler.h"
#include "UI.h"
#include "Semaphore.h"

struct MASTER_CONTROL_BLOCK {
	Scheduler* scheduler = new Scheduler();
	UI* ui = new UI();
	Semaphore* scheduler_semaphore = new Semaphore("UI Handler", 1);
	Semaphore* icp_semaphore = new Semaphore("ICP Handler", 1);
	
	MASTER_CONTROL_BLOCK(Scheduler* scheduler_, UI* ui_, Semaphore* scheduler_semaphore_, Semaphore* icp_semaphore_)
		: scheduler(scheduler_), ui(ui_), scheduler_semaphore(scheduler_semaphore_), icp_semaphore(icp_semaphore_) {}
}* master_control_block;

#endif