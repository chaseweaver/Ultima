#include "../inc/MemoryManager.h"

/*
 * MemoryManager::MemoryManager()
 * Default constructor.
 */
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

/*
 * MemoryManager::~MemoryManager()
 * Default deconstructor.
 */
MemoryManager::~MemoryManager() {}

/*
 * MemoryManager::find_hold(int)
 * Finds a valid hole in memory capable of supporting the requested size.
 * Returns a pointer to the hole, nullptr otherwise.
 */
MemoryManager::MEMORY_NODE* MemoryManager::find_hole(int size) {
	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();

		if (tmp_node->status == HOLE && tmp_node->limit - tmp_node->base >= size)
			return tmp_node;

	} while (!tmp->empty());

	return nullptr;
}


/*
 * MemoryManager::enough_blocks_exist(int)
 * Returns true if enough blocks exist as holes, false otherwise..
 */
bool MemoryManager::enough_blocks_exist(int size) {
	int tmp = 0;

	// Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	// do {
	// 	MEMORY_NODE* tmp_node = tmp->dequeue();

	// 	if (tmp_node->status == HOLE && tmp_node->limit - tmp_node->base >= size)
	// 		return tmp_node;

	// } while (!tmp->empty());

	return false;
}

/*
 * MemoryManager::allocate(const unsigned int)
 * Allocates a block of N size for a memory.
 * Returns -1 if a segfault occurs, a unique memory id otherwise.
 */
int MemoryManager::allocate(const unsigned int size) {
	int blocks_needed = size / block_size + (size % block_size != 0);
	// Checks if enough blocks are available
	// if (!enough_blocks_exist(size / block_size + (size % block_size != 0)));
	// 	return -1;

	// do {
		MEMORY_NODE* hole = find_hole(size * blocks_needed);

		// Returns an error if no holes are available
		if (hole == nullptr)
			return -1;

		MEMORY_NODE* mem = new MEMORY_NODE;

		mem->owner = pthread_self();
		mem->limit = hole->limit;
		mem->base = mem->limit - (block_size * blocks_needed);
		hole->limit = mem->base - 1;
		mem->handle = next_handle();
		mem->current_read = 0;
		mem->current_write = 0;
		mem->status = PROCESS;

		memory_list.enqueue(mem);
		return mem->handle;

	// } while (true);

	//return mem->handle;
}

/*
 * MemoryManager::read(int, char&)
 * Reads a char from a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 * The char is returned by reference.
 */
int MemoryManager::read(int memory_handle, char& ch) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;
			
		// Segfault
		if (tmp_node->base + tmp_node->current_read > tmp_node->limit)
			return -1;

		memory_core->read(tmp_node->base + tmp_node->current_read++, ch);
		
	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::read(int, std::string&)
 * Reads a string from a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 * The string is returned by reference.
 */
int MemoryManager::read(int memory_handle, std::string& str) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;
			
		// Segfault
		if (tmp_node->base + tmp_node->current_read > tmp_node->limit)
			return -1;

		for (int i = 0; i < str.size(); i++)
			memory_core->read(tmp_node->base + tmp_node->current_read++, str[i]);
			
	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::write(int, char)
 * Writes a char to a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, char ch) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;
			
		// Segfault
		if (tmp_node->base + tmp_node->current_write > tmp_node->limit)
			return -1;

		memory_core->write(tmp_node->base + tmp_node->current_write++, ch);

	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::write(int, std::string)
 * Writes a string to a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, std::string str) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue(); 
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;

		// Segfault
		if (tmp_node->base + tmp_node->current_write > tmp_node->limit + str.length())
			return -1;

		for (int i = 0; i < str.size(); i++)
			memory_core->write(tmp_node->base + tmp_node->current_write++, str[i]);

	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::write(int, int, std::string)
 * Writes a string to a block given a memory id with an offset.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, int offset_from_begin, std::string str) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue(); 
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;

		// Segfault
		if (tmp_node->base + offset_from_begin > tmp_node->limit)
			return -1;

		tmp_node->current_write = offset_from_begin;
		for (int i = 0; i < str.size(); i++)
			memory_core->write(tmp_node->base + tmp_node->current_write++, str[i]);

	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::write(int, int, char ch)
 * Writes a char to a block given a memory id with an offset.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, int offset_from_begin, char ch) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue(); 
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;

		// Segfault
		if (tmp_node->base + offset_from_begin > tmp_node->limit)
			return -1;

		tmp_node->current_write = offset_from_begin;
		memory_core->write(tmp_node->base + tmp_node->current_write++, ch);

	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::write(int, int, int, std::string)
 * Writes a char to a block given a memory id from START to END.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, int begin, int end, std::string str) {

	// Prevent writing to an invalid position
	if (memory_handle <= -1)
		return -1;

	// Prevent the writing of a string too large for the space requested
	if (end - begin <= str.length())
		return -1;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue(); 
		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;

		// Segfault
		if (tmp_node->base + begin > tmp_node->limit || tmp_node->base + end > tmp_node->limit)
			return -1;

		tmp_node->current_write = begin;
		for (int i = 0; i < str.length() && i < end; i++)
			memory_core->write(tmp_node->base + tmp_node->current_write++, str[i]);

	} while (!tmp->empty());
	return 1;
}

/*
 * MemoryManager::free(int)
 * Sets a block's status to HOLE and places '#' in all positions.
 */
void MemoryManager::free(int memory_handle) {
	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();

		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;

		tmp_node->status = HOLE;

		for (int i = 0; i < tmp_node->limit - tmp_node->base; i++)
			memory_core->write(tmp_node->base + i, '#');
			
	} while (!tmp->empty());

	coalesce();
}


// TODO: MAKE THIS WORK

/*
 * MemoryManager::coalesce()
 * Combines two or more contiguous blocks of free space. 
 */
void MemoryManager::coalesce() {
	int size = memory_list.size();

	if (size <= 1)
		return;

	do {
		MEMORY_NODE* tmp_node = memory_list.dequeue();
		MEMORY_NODE* tmp_sec = memory_list.dequeue();

		if (tmp_node->status == HOLE && tmp_sec->status == HOLE) {
			for (int i = 0; i < tmp_node->limit - tmp_node->base; i++)
				memory_core->write(tmp_node->base + i, '.');

			for (int i = 0; i < tmp_sec->limit - tmp_sec->base; i++)
				memory_core->write(tmp_sec->base + i, '.');

			tmp_sec->base = tmp_node->base;
			memory_list.enqueue(tmp_sec);
		}
			
	} while (--size > 1);
}

/*
 * MemoryManager::free_no_coalesce(int)
 * Sets a block's status to HOLE and places '#' in all positions.
 */
void MemoryManager::free_no_coalesce(int memory_handle) {
	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* tmp_node = tmp->dequeue();

		if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self())
			continue;

		tmp_node->status = HOLE;

		for (int i = 0; i < tmp_node->limit - tmp_node->base; i++)
			memory_core->write(tmp_node->base + i, '#');
			
	} while (!tmp->empty());
}

/*
 * MemoryManager::memory_largest()
 * Returns the largest block.
 */
int MemoryManager::memory_largest() {
	int largest = 0;

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* mem = tmp->dequeue();

		// Ignore unused blocks
		if (mem->status == HOLE)
			continue;

		// Finds the largest block size available
		if (largest <= mem->limit - mem->base)
			largest = mem->limit - mem->base;

	} while (!tmp->empty());

	return largest;
}

/*
 * MemoryManager::memory_smallest()
 * Returns the smallest block.
 */
int MemoryManager::memory_smallest() {
	int smallest = memory_left();

	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);
	do {
		MEMORY_NODE* mem = tmp->dequeue();

		// Ignore unused blocks
		if (mem->status == HOLE)
			continue;

		// Finds the smallest block size available
		if (smallest >= mem->limit - mem->base)
			smallest = mem->limit - mem->base;

	} while (!tmp->empty());

	return smallest;
}

/*
 * MemoryManager::memory_left()
 * Returns the amount of free space left in memory.
 */
int MemoryManager::memory_left() {
	return memory_core->memory_left();
}

/*
 * MemoryManager::next_handle()
 * Handles the memory ids.
 */
int MemoryManager::next_handle() {
	return ++memory_handle;
}

/*
 * MemoryManager::memory_dump(const unsigned int, const unsigned int)
 * Returns the memory contents from START to END.
 */
std::string MemoryManager::memory_dump(const unsigned int start, const unsigned int end) {
	return memory_core->memory_dump(start, end);
}

/*
 * MemoryManager::memory_dump()
 * Returns the memory contents from valid blocks.
 */
std::string MemoryManager::memory_dump() {
	std::string str = "";
	Queue<MEMORY_NODE*>* tmp = new Queue<MEMORY_NODE*>(memory_list);

	do {
		MEMORY_NODE* mem = tmp->dequeue();
		str += std::to_string(mem->handle) + " @ " + std::to_string(mem->limit - mem->base)
			+ " | " + memory_dump(mem->base, mem->limit - mem->base) + "\n";
	} while (!tmp->empty());

	return str;
}