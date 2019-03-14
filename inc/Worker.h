#ifndef WORKER_H
#define WORKER_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Struct.h"
#include "Semaphore.h"
#include "Scheduler.h"
#include <iostream>

class Worker {
private:
	MASTER_CONTROL_BLOCK* master_control_block;
	void* worker_function(void*);
	
public:
	Worker(MASTER_CONTROL_BLOCK*);
	~Worker();
	static void* start_worker_function(void*);
};

#endif
