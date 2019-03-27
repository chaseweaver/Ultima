#include "../inc/MemoryManager.h"

MemoryManager::MemoryManager(const unsigned int memory_size, const unsigned int memory_block_size, char null_character)
	: block_size(memory_block_size) {

	memory_core = new MemoryCore(memory_size, memory_block_size, null_character);

	memory_handle = 0;
	MEMORY_NODE* mem = new MEMORY_NODE;
	mem->base = 0;
	mem->limit = memory_size;
	mem->handle = -1;
	mem->status = HOLE;
	mem->owner = -1;
	memory_list.enqueue(mem);
}

MemoryManager::~MemoryManager() {}

MemoryManager::MEMORY_NODE* MemoryManager::find_a_hole(int size) {
	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();

		if (tmp_node->status == HOLE && tmp_node->limit - tmp_node->base >= size)
			return tmp_node;

	} while (!tmp->empty());

	return nullptr;
}

// Right approach
int MemoryManager::allocate(const unsigned int size) {
	if (size > block_size)
		return -1;

	MEMORY_NODE* hole = find_a_hole(size);

	// Returns an error if no holes are available
	if (hole == nullptr)
		return -1;

	MEMORY_NODE* mem = new MEMORY_NODE;

	mem->owner = pthread_self();
	mem->limit = hole->limit;
	mem->base = mem->limit - block_size;
	hole->limit = mem->base - 1;
	mem->handle = next_handle();
	mem->current_read = 0;
	mem->current_write = 0;

	memory_list.enqueue(mem);
	
	return mem->handle;
}

// TODO: Error handle for outside of bounds and SEG_FAULT
int MemoryManager::read(int memory_handle, char& ch) {

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();

		if (tmp_node->handle != memory_handle)
			break;
		
		// Call read function in CORE
		// ch = memory[tmp_node->current_read++];
			
	} while (!tmp->empty());

	return 1;
}

int MemoryManager::write(int memory_handle, char ch) {
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();
		if (tmp_node->handle == memory_handle) {
			
			// Segfault
			if (tmp_node->base + tmp_node->current_write >= tmp_node->limit)
				return -1;

			memory_core->write(tmp_node->base + tmp_node->current_write++, ch);
		}

	} while (!tmp->empty());

	return 1;
}

// TODO: Error handle for outside of bounds and SEG_FAULT
int MemoryManager::write(int memory_handle, std::string str) {
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue(); 
		if (tmp_node->handle == memory_handle) {

			// Segfault
			if (tmp_node->base + tmp_node->current_write >= tmp_node->limit + str.length())
				return -1;

			memory_core->write(tmp_node->base + tmp_node->current_write++, str);
		}

	} while (!tmp->empty());

	return 1;
}

//TODO: FINISH THIS SHIT
int MemoryManager::free(int memory_handle) {
	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();

		if (tmp_node->handle != memory_handle)
			break;

		tmp_node->status = HOLE;

		// Write fill here from CORE
			
	} while (!tmp->empty());

	return 1;
}

int MemoryManager::next_handle() {
	return ++memory_handle;
}

std::string MemoryManager::memory_dump(const unsigned int start, const unsigned int end) {
	return memory_core->memory_dump(start, end);
}

std::string MemoryManager::memory_dump() {
	return memory_core->memory_dump();
}