#include "../inc/MemoryManager.h"

MemoryManager::MemoryManager(int memory_size, char fill_character) {
	memory = new char[memory_size]{fill_character};
};