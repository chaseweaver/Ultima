#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include "MemoryCore.h"
#include <pthread.h>
#include <iostream>

#define HOLE 0
#define PROCESS 1

class MemoryManager {
private:
	struct MEMORY_NODE {
		pthread_t owner;
		int base;
		int limit;
		int handle;
		int status;
		int current_read;
		int current_write;
	};

	MemoryCore* memory_core;
	const int block_size;
	int memory_handle;

	Queue<MEMORY_NODE*> memory_list;
	int next_handle();

public:
	MemoryManager(const unsigned int, const unsigned int, char);
	~MemoryManager();
	MEMORY_NODE* find_a_hole(int);
	int allocate(const unsigned int);
	int read(int, char&);
	int read(int, std::string&);
	int write(int, char);
	int write(int, std::string);
	int free(int);
	int memory_left();
	int memory_largest();
	int memory_smallest();
	std::string memory_dump(const unsigned int, const unsigned int);
	std::string memory_dump();
};

#endif