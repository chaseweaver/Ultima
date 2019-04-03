#include "../inc/MemoryCore.h"

MemoryCore::MemoryCore(const unsigned int memory_size,
											 const unsigned int memory_block_size,
											 char								fill_character)
	: memory_capacity(memory_size), free_capacity(memory_size), block_size(memory_block_size),
		null_character(fill_character) {
	memory = new char[memory_size];
	std::fill_n(memory, memory_size, fill_character);
}

void MemoryCore::write(const unsigned int location, char ch) {
	memory[location] = ch;
	--free_capacity;
}

void MemoryCore::write_free(const unsigned int location, char ch) {
	memory[location] = ch;
	++free_capacity;
}

void MemoryCore::read(const unsigned int location, char& ch) { ch = memory[location]; }

std::string MemoryCore::memory_dump(const unsigned int start, const unsigned int end) {
	return std::string(memory).substr(start, end);
}

std::string MemoryCore::memory_dump() { return std::string(memory); }

int MemoryCore::memory_size() { return memory_capacity; }

int MemoryCore::memory_blocks() { return memory_capacity / block_size; }

int MemoryCore::memory_left() { return free_capacity; }