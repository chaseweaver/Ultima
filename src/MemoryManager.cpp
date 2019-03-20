#include "../inc/MemoryManager.h"

MemoryManager::MemoryManager(const unsigned int memory_size, const unsigned int memory_block_size, char fill_character)
	: memory_capacity(memory_size), free_capacity(memory_size), block_size(memory_block_size) {
	memory = new char[memory_size];
	std::fill_n(memory, memory_size, fill_character);
}

MemoryManager::~MemoryManager() {
	delete(memory);
}

int MemoryManager::allocate(int size) {

}

std::string MemoryManager::memory_dump(int start, int end) {
	return std::string(memory).substr(start, end);
}

std::string MemoryManager::memory_dump() {
	return std::string(memory);
}