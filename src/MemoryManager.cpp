#include "../inc/MemoryManager.h"

MemoryManager::MemoryManager(const unsigned int memory_size, const unsigned int memory_block_size, char fill_character)
	: memory_capacity(memory_size), free_capacity(memory_size), block_size(memory_block_size), null_character(fill_character) {
	memory = new char[memory_size];
	std::fill_n(memory, memory_size, fill_character);

	memory_sectors = memory_capacity / block_size;
}

MemoryManager::~MemoryManager() {
	delete(memory);
}

int MemoryManager::allocate(int size) {
	int index = 0, index_last = 0;
	int bounds = sizeof(memory) / sizeof(*memory);
	bool valid;

	for (int i = 0; i < bounds; i += block_size) {
		if (memory[i] == null_character) {
			index = i;

			for (int j = index + 1; j < size, j < bounds; j++) {
				if (memory[j] != null_character) {
					valid = false;
					break;
				} else {
					index_last = j;
					valid = true;
				}
			}

			if (valid)
				break;
		}
	}

	if (valid) {
		MEMORY_DATA* mem = new MEMORY_DATA;
		mem->start_loc = index;
		mem->end_loc = index + block_size;
		mem->current_loc = index_last;
		mem->memory_size = size;
		mem->memory_id = ++memory_counter;
		memory_data.enqueue(mem);

		return memory_counter;
	}

	return -1;
}

void MemoryManager::write(int memory_id, std::string data) {
	Queue<MEMORY_DATA*> tmp_mem_dat = Queue<MEMORY_DATA*>(memory_data);

	for (int i = 0; i < tmp_mem_dat.size(); i++) {
		MEMORY_DATA* mem = tmp_mem_dat.dequeue();
		
		if (mem->memory_id == memory_id)
			for (int j = 0; j < data.length(); j++)
				memory[mem->start_loc + j] = data[j];
	}
}

std::string MemoryManager::memory_dump(int start, int end) {
	return std::string(memory).substr(start, end);
}

std::string MemoryManager::memory_dump() {
	return std::string(memory);
}

// bool MemoryManager::is_sector_valid(int size) {
// 	return false;
// }