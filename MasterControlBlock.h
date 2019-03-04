#ifndef MASTER_CONTROL_BLOCK_H
#define MASTER_CONTROL_BLOCK_H

#ifdef _MSC_VER
#pragma once
#endif

#include "Scheduler.h"
#include "UI.h"
#include "Semaphore.h"

struct MASTER_CONTROL_BLOCK {
	const Scheduler* scheduler = new Scheduler();
	const UI* ui = new UI();
	const Semaphore* scheduler_semaphore = new Semaphore("UI Handler", 1);
	const Semaphore* icp_semaphore = new Semaphore("ICP Handler", 1);

	MASTER_CONTROL_BLOCK(Scheduler* const& scheduler_, UI* const& ui_,
		Semaphore* const& scheduler_semaphore_, Semaphore* const& icp_semaphore_)
			: scheduler(scheduler_), ui(ui_), scheduler_semaphore(scheduler_semaphore_), icp_semaphore(icp_semaphore_) {}
};

#endif