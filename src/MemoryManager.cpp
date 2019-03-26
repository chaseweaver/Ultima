#include "../inc/MemoryManager.h"

MemoryManager::MemoryManager(const unsigned int memory_size, const unsigned int memory_block_size, char fill_character)
	: memory_capacity(memory_size), free_capacity(memory_size), block_size(memory_block_size), null_character(fill_character) {
	memory = new char[memory_size];
	std::fill_n(memory, memory_size, fill_character);
}

MemoryManager::~MemoryManager() {
	delete(memory);
}

int MemoryManager::allocate(int size) {
	int blocks_to_allocate = size / block_size + (size % block_size != 0);
	int tmp_block = blocks_to_allocate;

	// Return an error if not enough requested space is available
	if (!enough_blocks_available(blocks_to_allocate))
		return -1;

	// Incrememnt memory id
	++memory_counter;

	// Check index of core memory every block_size
	for (int i = 0; i < memory_capacity, blocks_to_allocate != 0; i+= block_size) {
		if (memory[i] != null_character)
			break;

		// Set the current location equal to the index
		current_loc = i;

		MEMORY_DATA* mem = new MEMORY_DATA;
		mem->start_loc = i;
		mem->memory_id = memory_counter;
		
		// No bueno?
		mem->memory_size = (blocks_to_allocate != 0 ? size -= block_size : size);

		memory_data.enqueue(mem);

		free_capacity -= size;
		--blocks_to_allocate;
	}

	return memory_counter;
}

void MemoryManager::write(int memory_id, std::string data) {
	
	Queue<MEMORY_DATA*> tmp = Queue<MEMORY_DATA*>(memory_data);
	int tmp_ctr = 0;
	bool more_to_put = true;

	for (int i = 0; i < tmp.size(); i++) {
		MEMORY_DATA* mem = tmp.dequeue();
		
		if (mem->memory_id == memory_id) {

			for (int j = 0; j < data.length(); j++) {
				memory[mem->start_loc + j] = data[j];
				
				// more_to_put = data.length() - j > 0 ? true : false;	
			}	
		}
		
	}
}

std::string MemoryManager::memory_dump(int start, int end) {
	return std::string(memory).substr(start, end);
}

std::string MemoryManager::memory_dump() {
	return std::string(memory);
}

bool MemoryManager::enough_blocks_available(int requested_block_size) {
	int tmp = 0;
	for (int i = 0; i < memory_capacity; i+= block_size)
		if (memory[i] == null_character)
			++tmp;

	return tmp >= requested_block_size;
}