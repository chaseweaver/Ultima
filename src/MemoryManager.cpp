#include "../inc/MemoryManager.h"

/*
 * MemoryManager::MemoryManager(const unsigned int, const unsigned int, char)
 * Default constructor.
 */
MemoryManager::MemoryManager(const unsigned int memory_size,
  const unsigned int memory_block_size,
  char null_character)
  : block_size(memory_block_size) {
  mem_core = new MemoryCore(memory_size, memory_block_size, null_character);
  mem_sema = new Semaphore(mcb, "Memory Manager", 1);

  memory_handle = 0;
  MEMORY_NODE* mem = new MEMORY_NODE;
  mem->base = 0;
  mem->limit = memory_size;
  mem->handle = -1;
  mem->status = HOLE;

  mem_sema->wait();
  memory_list.enqueue(mem);
  mem_sema->signal();
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
  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();

    if (tmp_node->status == HOLE && tmp_node->limit - tmp_node->base >= size)
      return tmp_node;

  } while (!tmp->empty());
  return nullptr;
}

/*
 * MemoryManager::allocate(const unsigned int)
 * Allocates a block of N size for a memory.
 * Returns -1 if a segfault occurs, a unique memory id otherwise.
 */
int MemoryManager::allocate(const unsigned int size) {
  int blocks_needed = size / block_size + (size % block_size != 0);

  MEMORY_NODE* hole = find_hole(size * blocks_needed);

  // Returns an error if no holes are available
  if (hole == nullptr) return -1;

  MEMORY_NODE* mem = new MEMORY_NODE;

  mem->owner = pthread_self();
  mem->limit = hole->limit;
  mem->base = mem->limit - (block_size * blocks_needed);
  hole->limit = mem->base - 1;
  mem->handle = next_handle();
  mem->current_read = 0;

  mem->current_write = 0;
  mem->status = PROCESS;

  mem_sema->wait();
  memory_list.enqueue(mem);
  mem_sema->signal();

  return mem->handle;
}

/*
 * MemoryManager::read(int, char&)
 * Reads a char from a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 * The char is returned by reference.
 */
int MemoryManager::read(int memory_handle, char& ch) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + tmp_node->current_read > tmp_node->limit) return -1;

    mem_core->read(tmp_node->base + tmp_node->current_read++, ch);
  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::read(int, std::string&)
 * Reads a string from a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 * The string is returned by reference.
 */
int MemoryManager::read(int memory_handle, int size, std::string& str) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + tmp_node->current_read > tmp_node->limit) return -1;

    for (int i = 0; i < str.size(), i < size; i++)
      mem_core->read(tmp_node->base + tmp_node->current_read++, str[i]);

  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::write(int, char)
 * Writes a char to a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, char ch) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + tmp_node->current_write > tmp_node->limit) return -1;

    mem_core->write(tmp_node->base + tmp_node->current_write++, ch);
  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::write(int, std::string)
 * Writes a string to a block given a memory id.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, std::string str) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + tmp_node->current_write > tmp_node->limit + str.length())
      return -1;

    for (int i = 0; i < str.size(); i++)
      mem_core->write(tmp_node->base + tmp_node->current_write++, str[i]);

  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::write(int, int, std::string)
 * Writes a string to a block given a memory id with an offset.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, int offset_from_begin, std::string str) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + offset_from_begin > tmp_node->limit) return -1;

    tmp_node->current_write = offset_from_begin;
    for (int i = 0; i < str.size(); i++)
      mem_core->write(tmp_node->base + tmp_node->current_write++, str[i]);

  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::write(int, int, char ch)
 * Writes a char to a block given a memory id with an offset.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, int offset_from_begining, char ch) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + offset_from_begining > tmp_node->limit) return -1;

    tmp_node->current_write = offset_from_begining;
    mem_core->write(tmp_node->base + tmp_node->current_write++, ch);

  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::write(int, int, int, std::string)
 * Writes a char to a block given a memory id from START to END.
 * Returns -1 if a segfault occurs, 1 otherwise.
 */
int MemoryManager::write(int memory_handle, int begin, int end, std::string str) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return -1;

  // Prevent the writing of a string too large for the space requested
  if (end - begin <= str.length()) return -1;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  mem_sema->wait();

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;
    if (tmp_node->base + begin > tmp_node->limit ||
      tmp_node->base + end > tmp_node->limit)
      return -1;

    tmp_node->current_write = begin;
    for (int i = 0; i < str.length() && i < end; i++)
      mem_core->write(tmp_node->base + tmp_node->current_write++, str[i]);

  } while (!tmp->empty());
  mem_sema->signal();

  return 1;
}

/*
 * MemoryManager::free(int)
 * Sets a block's status to HOLE and places '#' in all positions.
 */
void MemoryManager::free(int memory_handle) {

  // Prevent writing to an invalid position
  if (memory_handle <= -1) return;

  mem_sema->wait();
  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();

    if (tmp_node->handle != memory_handle || tmp_node->owner != pthread_self()) continue;

    tmp_node->status = HOLE;

    for (int i = 0; i < tmp_node->limit - tmp_node->base + 1; i++)
      mem_core->write_free(tmp_node->base + i, '#');

  } while (!tmp->empty());
  mem_sema->signal();

  coalesce(memory_handle);
}

/*
 * MemoryManager::coalesce(int)
 * Combines two or more contiguous blocks of free space
 * based on the memory_handle.
 */
void MemoryManager::coalesce(int memory_handle) {
  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  Queue< MEMORY_NODE* >* tmp_2 = new Queue< MEMORY_NODE* >(memory_list);
  Queue< MEMORY_NODE* >* tmp_3 = new Queue< MEMORY_NODE* >(memory_list);

  MEMORY_NODE* tmp_node = nullptr;
  MEMORY_NODE* tmp_node_2 = nullptr;
  MEMORY_NODE* tmp_node_3 = nullptr;

  bool node_found = false;
  bool node_found_2 = false;
  bool node_found_3 = false;
  bool with_original_block = false;

  do {
    tmp_node = tmp->dequeue();

    if (tmp_node->handle != memory_handle) continue;

    node_found = true;
  } while (!tmp->empty() && node_found == false);

  // Find node that is to the left (in memory) of the current node
  do {
    tmp_node_2 = tmp_2->dequeue();

    if ((tmp_node_2->limit == tmp_node->base - 1) && (tmp_node_2->status == HOLE)) {
      node_found_2 = true;
      if (tmp_node_2->handle == -1) with_original_block = true;
    }

  } while (!tmp_2->empty() && node_found_2 == false);

  // Find node that is to the right (in memory) of the current node
  do {
    tmp_node_3 = tmp_3->dequeue();

    if ((tmp_node_3->base == tmp_node->limit + 1) && (tmp_node_3->status == HOLE)) {
      node_found_3 = true;
      if (tmp_node_3->handle == -1) with_original_block = true;
    }

  } while (!tmp_3->empty() && node_found_3 == false);

  if (!node_found) return;

  if (node_found_2 && node_found_3) {
    tmp_node->base = tmp_node_2->base;
    tmp_node->limit = tmp_node_3->limit;

    mem_sema->wait();
    for (int i = tmp_node->base; i <= tmp_node->limit; i++)
      mem_core->write(tmp_node->base + i, '.');
    mem_sema->signal();

    tmp_node->current_read = 0;
    tmp_node->current_write = 0;

    if (with_original_block) tmp_node->handle = -1;

    tmp_node_2->base = 0;
    tmp_node_2->limit = 0;

    tmp_node_3->base = 0;
    tmp_node_3->limit = 0;

  } else if (node_found_2) {
    tmp_node->base = tmp_node_2->base;

    mem_sema->wait();
    for (int i = tmp_node->base; i <= tmp_node->limit; i++)
      mem_core->write(tmp_node->base + i, '.');
    mem_sema->signal();

    tmp_node->current_read = 0;
    tmp_node->current_write = 0;

    if (with_original_block) tmp_node->handle = -1;

    tmp_node_2->base = 0;
    tmp_node_2->limit = 0;

  } else if (node_found_3) {
    tmp_node->limit = tmp_node_3->limit;

    mem_sema->wait();
    for (int i = tmp_node->base; i <= tmp_node->limit; i++)
      mem_core->write(tmp_node->base + i, '.');
    mem_sema->signal();

    tmp_node->current_read = 0;
    tmp_node->current_write = 0;

    if (with_original_block) tmp_node->handle = -1;

    tmp_node_3->base = 0;
    tmp_node_3->limit = 0;
  }

  memory_cleanup();
}

/*
 * MemoryManager::memory_cleanup()
 * Reorganizes the memory queue order.
 */
void MemoryManager::memory_cleanup() {
  MEMORY_NODE* tmp_node = nullptr;

  mem_sema->wait();

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >();
  int size = memory_list.size();

  do {
    tmp_node = memory_list.dequeue();
    if (tmp_node->limit - tmp_node->base > 0)
      if (tmp_node->handle == -1)
        memory_list.enqueue(tmp_node);
      else
        tmp->enqueue(tmp_node);
  } while (--size != 0);

  while (!tmp->empty()) {
    tmp_node = tmp->dequeue();
    memory_list.enqueue(tmp_node);
  }

  mem_sema->signal();
}

/*
 * MemoryManager::memory_largest()
 * Returns the largest block.
 */
int MemoryManager::memory_largest() {
  int largest = 0;

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  do {
    MEMORY_NODE* mem = tmp->dequeue();

    // Ignore unused blocks
    if (mem->status == HOLE) continue;

    // Finds the largest block size available
    if (largest <= mem->limit - mem->base) largest = mem->limit - mem->base;

  } while (!tmp->empty());

  return largest;
}

/*
 * MemoryManager::memory_smallest()
 * Returns the smallest block.
 */
int MemoryManager::memory_smallest() {

  mem_sema->wait();
  int smallest_segment = mem_core->memory_size();
  mem_sema->signal();

  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->status == HOLE)
      if (smallest_segment >= tmp_node->limit - tmp_node->base)
        smallest_segment = tmp_node->limit - tmp_node->base;

  } while (!tmp->empty());

  return smallest_segment;
}

/*
 * MemoryManager::memory_left()
 * Returns the amount of free space left in memory.
 */
int MemoryManager::memory_left() {
  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);
  int mem_left_amnt = 0;

  do {
    MEMORY_NODE* tmp_node = tmp->dequeue();
    if (tmp_node->status == HOLE) mem_left_amnt += tmp_node->limit - tmp_node->base;
  } while (!tmp->empty());
  return mem_left_amnt;
}

/*
 * MemoryManager::next_handle()
 * Handles the memory ids.
 */
int MemoryManager::next_handle() { return ++memory_handle; }

/*
 * MemoryManager::memory_dump(const unsigned int, const unsigned int)
 * Returns the memory contents from START to END.
 */
std::string MemoryManager::memory_dump(const unsigned int start, const unsigned int end) {
  return mem_core->memory_dump(start, end);
}

/*
 * MemoryManager::memory_dump()
 * Returns the memory contents from valid blocks.
 * Includes the memory handle as well as allocated space.
 */
std::string MemoryManager::memory_dump() {
  std::string str = "";
  Queue< MEMORY_NODE* >* tmp = new Queue< MEMORY_NODE* >(memory_list);

  do {
    MEMORY_NODE* mem = tmp->dequeue();

    str += (mem->handle == -1 ? "Free" : std::to_string(mem->handle)) + " @ " +
      std::to_string(mem->limit - mem->base) + " | " +
      memory_dump(mem->base, mem->limit - mem->base) + "\n";
  } while (!tmp->empty());

  return str;
}

/*
 * MemoryManager::memory_dump_mem()
 * Returns the memory contents from valid blocks.
 */
std::string MemoryManager::memory_dump_mem() {
  return "Main Memory @ " + std::to_string(mem_core->memory_size()) + " | " +
    mem_core->memory_dump() + "\n";
}
