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
	MASTER_CONTROL_BLOCK() {}
}* master_control_block = new MASTER_CONTROL_BLOCK();

#endif