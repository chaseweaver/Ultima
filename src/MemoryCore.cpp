#include "../inc/MemoryCore.h"

/*
 * MemoryCore::MemoryCore(const unsigned int, const unsigned int, char)
 * Default constructor.
 */
MemoryCore::MemoryCore(const unsigned int memory_size,
  const unsigned int memory_block_size,
  char fill_character)
  : memory_capacity(memory_size), free_capacity(memory_size),
    block_size(memory_block_size), null_character(fill_character) {
  memory = new char[memory_size];
  std::fill_n(memory, memory_size, fill_character);
}

/*
 * MemoryCore::write(const unsigned int, char)
 * Places a char in the memory array based on a requested index.
 * Decrements the current free space.
 */
void MemoryCore::write(const unsigned int location, char ch) {
  memory[location] = ch;
  --free_capacity;
}

/*
 * MemoryCore::write(const unsigned int, char)
 * Places a char in the memory array based on a requested index.
 * Increments the current free space.
 */
void MemoryCore::write_free(const unsigned int location, char ch) {
  memory[location] = ch;
  ++free_capacity;
}

/*
 * MemoryCore::read(const unsigned int, char&)
 * Sets the reference char equal to the char in the memory array at a requested index.
 */
void MemoryCore::read(const unsigned int location, char& ch) { ch = memory[location]; }

/*
 * MemoryCore::memory_dump(const unsigned int, const unsigned int)
 * Returns a string of contents of the memory array from a start and end point.
 */
std::string MemoryCore::memory_dump(const unsigned int start, const unsigned int end) {
  return std::string(memory).substr(start, end);
}

/*
 * MemoryCore::memory_dump()
 * Returns a string of contents of the memory array.
 */
std::string MemoryCore::memory_dump() { return std::string(memory); }

/*
 * MemoryCore::memory_blocks()
 * Returns the amount of blocks in the memory array.
 */
int MemoryCore::memory_blocks() { return memory_capacity / block_size; }

/*
 * MemoryCore::memory_size()
 * Returns the amount of memory capacity originally allocated.
 */
int MemoryCore::memory_size() { return memory_capacity; }

/*
 * MemoryCore::memory_left()
 * Returns the amount of free capacity.
 */
int MemoryCore::memory_left() { return free_capacity; }