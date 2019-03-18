#ifndef STRUCT_H
#define STRUCT_H

#ifdef _MSC_VER
#pragma once
#endif

#include <iostream>
#include <pthread.h>

struct ARGUMENTS {
	int id;
	int thread_results;
	struct TASK_CONTROL_BLOCK* task_control_block;
};

struct TASK_CONTROL_BLOCK {
	int task_id;
	int task_state;
	std::string task_name;
	pthread_t task_thread;
	ARGUMENTS* task_arguments;
};

#endif