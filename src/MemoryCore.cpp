#include "../inc/MemoryCore.h"

MemoryCore::MemoryCore(const unsigned int memory_size, const unsigned int memory_block_size, char fill_character)
	: memory_capacity(memory_size), block_size(memory_block_size), null_character(fill_character) {
	memory = new char[memory_size];
	std::fill_n(memory, memory_size, fill_character);
}

void MemoryCore::write(const unsigned int location, char ch) {
	memory[location] = ch;
}

void MemoryCore::read(const unsigned int location, char& ch) {
	ch = memory[location];
}

void MemoryCore::write(const unsigned int location, std::string str) {
	for (int i = 0; i < str.size(); i++)
		memory[location + i] = str[i];
}

std::string MemoryCore::memory_dump(const unsigned int start, const unsigned int end) {
	return std::string(memory).substr(start, end);
}

std::string MemoryCore::memory_dump() {
	return std::string(memory);
}