#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#ifdef _MSC_VER
#pragma once
#endif 

struct MASTER_CONTROL_BLOCK;
#include "MasterControlBlock.h"

#include "Queue.h"
#include <iostream>

#define USED 0
#define FREE 1

class MemoryManager {
private:
	struct MEMORY_DATA {
		int status;
		int memory_id;
		int start_loc;
		int end_loc;
		int memory_size;
		int current_loc;
		int task_id;
	};

	char* memory;
	const int memory_capacity;
	const int block_size;
	int null_character;

	int memory_sectors;
	int free_capacity;
	int used_capacity;
	int memory_counter;

	Queue<MEMORY_DATA*> memory_data;

	// bool is_sector_valid(int);

public:
	MemoryManager(const unsigned int, const unsigned int, char);
	~MemoryManager();
	int allocate(int);
	void write(int, std::string);
	std::string memory_dump(int, int);
	std::string memory_dump();
};

#endif