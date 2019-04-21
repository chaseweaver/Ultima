#include "../inc/UFS.h"

/*
 * UFS::UFS(MASTER_CONTROL_BLOCK*)
 * Default constructor.
 */
UFS::UFS(MASTER_CONTROL_BLOCK* mcb,
  std::string filesystem_name,
  int num_blocks,
  int size_block,
  char init_char)
  : mcb(mcb), fs_name(filesystem_name), fs_number_of_blocks(num_blocks),
    fs_block_size(size_block), initilization_char(init_char) {

  ufs_sema = new Semaphore(mcb, "UFS Handler", 1);
  file_system = new char[num_blocks * size_block];
  std::fill_n(file_system, num_blocks * size_block, init_char);
  next_file_handle = 0;
  next_unique_file_handle = 0;
  format();
  init_inodes();
}

/*
 * UFS::~UFS()
 * Default deconstructor.
 */
UFS::~UFS() {}

/*
 * UFS::format()
 *
 */
void UFS::format() {
  std::fstream disk("./disk/disk.txt", std::ios::out);
  for (int i = 0; i < fs_block_size * fs_number_of_blocks; i++) {
    disk.seekp(i, std::ios::beg);
    disk.put(initilization_char);
  }

  disk.close();
}

/*
 * UFS::enough_inodes_available(int)
 * Returns true if enough empty blocks are available, false otherwise.
 */
bool UFS::enough_inodes_available(int num_of_blocks) {
  return true;
  // Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
}

/*
 * UFS::change_permission(int, std::string, int, char[4])
 * Changes the permission of a file if file OWNER.
 */
int UFS::change_permission(int file_handle, std::string name, char perm[4]) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  bool success = false;

  do {
    INODE* node = tmp->dequeue();
    if (node->file_id != file_handle || node->owner != pthread_self()) {
      success = false;
      continue;
    }
    for (int i = 0; i < 4; i++) node->permission[i] = perm[i];
    success = true;
  } while (!tmp->empty());
  return success;
}

/*
 * UFS::delete_file(std::string, int, char[4])
 *
 */
int UFS::delete_file(int file_handle, std::string name) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  bool success = false;
  int size = tmp->size();

  ufs_sema->wait();

  do {
    INODE* node = tmp->dequeue();
    if (node->owner == pthread_self() &&
      (node->file_id == file_handle || node->filename == name)) {

      node->filename = "--------";
      node->active = false;
      node->current_write = 0;
      node->current_read = 0;
      node->file_id = 0;
      node->size = 0;
      for (int i = 0; i < 4; i++) node->permission[i] = '-';

      std::fstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
      disk.seekp(std::ios::beg);
      disk.seekg(std::ios::beg);
      for (int i = 0; i < fs_block_size; i++) {
        disk.seekp(i);
        disk.put('$');
      }

      disk.flush();
      disk.close();

      success = true;
    } else {
      success = false;
    }
  } while (--size != 0);

  ufs_sema->signal();
  write_inodes();
  return success;
}

/*
 * UFS::create_file(std::string, int, char[4])
 *
 */
int UFS::create_file(std::string name, int file_size, char perm[4]) {
  if (file_size > 4 * fs_block_size) return -1;

  int blocks_needed = file_size / fs_block_size + (file_size % fs_block_size != 0);
  int blocks_remaining = blocks_needed;

  if (!enough_inodes_available(blocks_needed)) return -1;

  std::chrono::milliseconds t =
    duration_cast< milliseconds >(system_clock::now().time_since_epoch());
  int handle = next_handle();

  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  do {
    if (blocks_remaining == 0) break;

    INODE* node = tmp->dequeue();
    if (node->active) continue;

    node->filename = name;
    for (int i = 0; i < 4; i++) node->permission[i] = perm[i];

    node->owner = pthread_self();
    node->file_id = handle;
    node->active = true;
    node->creation_time = t;
    node->last_modified_time = t;

    --blocks_remaining;
    write_inodes();

  } while (!tmp->empty());
  return handle;
}

// TF is this even supposed to do?

/*
 * UFS::open(int, std::string, char)
 *
 */
int UFS::open(int file_handle, std::string name, char mode) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();

  do {
    INODE* node = tmp->dequeue();

    // SUPER FIX THIS
    if (node->owner == pthread_self() ||
      (node->filename == name) &&
        (node->permission[2] == mode || node->permission[3] == mode))
      return (node->file_id == file_handle);

  } while (--size != 0);
  return -1;
}

/*
 * UFS::return_inode(int)
 * Returns the INODE at the requested position.
 */
UFS::INODE* UFS::return_inode(int file_handle) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);

  do {
    INODE* node = tmp->dequeue();
    if (node->file_id == file_handle) return node;
  } while (!tmp->empty());

  return nullptr;
}

/*
 * UFS::write_char(int, char)
 * Writes a char to a file at the current write position.
 * Returns -1 if an error occurs, 1 otherwise.
 */
int UFS::write_char(int file_handle, char ch) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  std::ofstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
  disk.seekp(node->block_id * fs_block_size + node->current_write++);
  disk.put(ch);
  ++node->size;

  node->last_modified_time = node->last_modified_time =
    duration_cast< milliseconds >(system_clock::now().time_since_epoch());

  write_inodes();
  disk.close();
  return 1;
}

/*
 * UFS::enough_registered_inodes(int)
 * Valiation check to ensure enough INODEs with registered IDs are available.
 */
bool UFS::enough_registered_inodes(int num_of_nodes) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int count = 0;

  do {
    INODE* node = tmp->dequeue();

    if (!node->active) ++count;
    if (count == num_of_nodes) return true;
  } while (!tmp->empty());

  return count == num_of_nodes;
}

/*
 * UFS::write_string(int, std::string)
 * Writes a string to a file at the current write position.
 * Returns -1 if an error occurs, 1 otherwise.
 */
int UFS::write_string(int file_handle, std::string str) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  char ch[str.length() + 1];
  for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  std::fstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
  int offset = node->block_id * fs_block_size;
  for (int i = 0; i < strlen(ch); i++) {
    disk.seekp(offset + node->current_write++, std::ios::beg);
    disk.put(ch[i]);
    ++node->size;
  }

  node->last_modified_time = node->last_modified_time =
    duration_cast< milliseconds >(system_clock::now().time_since_epoch());

  write_inodes();
  disk.close();
  return 1;
}

/*
 * UFS::read_char(int, char*)
 * Seeks and sets a char pointer equal to the current read character
 * in a file.
 * Returns -1 if an error occurs, 1 otherwise.
 */
int UFS::read_char(int file_handle, char* ch) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  // std::string name(node->filename);

  std::ifstream disk("./disk/disk.txt", std::ios::out);
  disk.seekg(node->block_id * fs_block_size + node->current_read++, std::ios::beg);
  char c;
  disk.get(c);
  ch = &c;

  disk.close();

  return 1;
}

/*
 * UFS::next_handle()
 * Increments and returns a file handle.
 */
int UFS::next_handle() { return ++next_file_handle; }

/*
 * UFS::next_unique_handle()
 * Increments and returns a unique file handle.
 */
int UFS::next_unique_handle() { return ++next_unique_file_handle; }

/*
 * UFS::init_inodes()
 * Initializes INODE structures with default values.
 */
void UFS::init_inodes() {
  for (int i = 0; i < fs_number_of_blocks; i++) {
    INODE* node = new INODE;

    node->filename = "--------";
    node->owner = pthread_self();
    node->block_id = i;
    node->size = 0;

    for (int j = 0; j < 4; j++) node->permission[j] = '-';
    node->active = false;
    node->current_read = 0;
    node->current_write = 0;

    nodes.enqueue(node);
  }
}

/*
 * UFS::write_inodes()
 * Writes INODE structures to a disk.
 */
void UFS::write_inodes() {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();
  std::string str = "";

  do {
    INODE* node = tmp->dequeue();
    if (!node->active) continue;
    str += deconstruct_inode(node);
  } while (--size != 0);
  char ch[str.length() + 1];
  for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  std::fstream disk("./disk/inode_fs.txt", std::ios::out | std::ios::trunc);
  disk.seekp(std::ios::beg);
  disk.write(ch, strlen(ch));

  disk.close();
}

/*
 * UFS::amount_of_inodes()
 * Returns the amout of INODEs on a disk.
 */
int UFS::amount_of_inodes() { return nodes.size(); }

/*
 * UFS::deconstruct_inode(INODE*)
 * Converts an INODE structure into a binary string.
 */
std::string UFS::deconstruct_inode(INODE* node) {
  std::string str = "";

  for (int i = 0; i < 16; i++)
    str += std::bitset< 8 >(node->filename[i] * sizeof(char)).to_string();

  str += std::bitset< 32 >(node->owner).to_string() + ' ';
  str += std::bitset< 8 >(node->block_id).to_string() + ' ';
  str += std::bitset< 8 >(node->size).to_string() + ' ';

  for (int i = 0; i < 4; i++)
    str += std::bitset< 4 >(node->permission[i] * sizeof(char)).to_string();

  str += ' ';
  str += std::bitset< 1 >(node->active).to_string() + ' ';
  str += std::bitset< 8 >(node->current_read).to_string() + ' ';
  str += std::bitset< 8 >(node->current_write).to_string() + ' ';
  str += std::bitset< 8 >(node->file_id).to_string() + ' ';
  str += std::bitset< 32 >(node->creation_time.count()).to_string() + ' ';
  str += std::bitset< 32 >(node->last_modified_time.count()).to_string();
  str += '\n';
  return str;
}

// This does not work

/*
 * UFS::construct_inode(std::string)
 * Converts a binary string back into an INODE structure.
 */
UFS::INODE* UFS::construct_inode(std::string bin) {
  int filename_size = 128;
  int owner_size = 32;
  int block_id_size = 8;
  int size_size = 8;
  int permission_size = 16;
  int active_size = 1;
  int current_read_size = 8;
  int current_write_size = 8;
  int id_size = 8;
  int creation_time_size = 32;
  int last_modified_size = 32;

  INODE* node = new INODE;

  for (int i = 0; i < filename_size; i++) {
    std::string str = "";
    for (int j = i; j < filename_size / 8; j++) str += bin[j];
    node->filename = std::stoi(str, nullptr, 2);
  }

  return nullptr;
}

/*
 * UFS::read_inodes()
 * Reads INODEs from a disk.
 */
std::string UFS::read_inodes() {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();
  std::string str = "";

  // do { str += deconstruct_inode(tmp->dequeue()); } while (--size != 0);
  // char ch[str.length() + 1];
  // for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  // std::fstream disk("./disk/inode_fs.txt", std::ios::out);
  // disk.seekp(std::ios::beg);
  // disk.write(ch, strlen(ch));

  // disk.close();

  return "";
}

/*
 * UFS::char_to_binary(unsigned char)
 * Converts a char to a binary string.
 */
std::string UFS::char_to_binary(unsigned char value) {
  char theResult[128];
  unsigned char mask = 0x80;
  int i;

  for (i = 0; i < sizeof(value) * 8; i++) {
    ((value & mask) == 0) ? theResult[i] = '0' : theResult[i] = '1';
    mask >>= 1;
  }
  theResult[i] = '\0';
  std::string result = theResult;
  return (result);
}

/*
 * UFS::dir()
 * Lists the contents of INODE structures.
 */
std::string UFS::dir() {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);

  if (tmp->empty()) return "\n There are no valid INODE structures!";
  int ctr = 0;

  std::string str = "\n INODE Directory Structure\n\n ";
  std::string name_title = "Filename";
  std::string perm_title = "Perms";
  std::string size_title = "Size";
  std::string owner_title = "Owner";
  std::string created_time_tile = "Time Created";
  std::string last_mod_tile = "Last Modified";
  std::string pbreak = " ";
  pad(pbreak, 77, '-');

  pad(name_title, 9, ' ');
  pad(perm_title, 5, ' ');
  pad(size_title, 5, ' ');
  pad(owner_title, 15, ' ');
  pad(created_time_tile, 14, ' ');
  pad(last_mod_tile, 13, ' ');

  str += name_title + " | " + perm_title + " | " + size_title + " | " + owner_title +
    " | " + created_time_tile + " | " + last_mod_tile + "\n" + pbreak + "\n";

  do {
    INODE* node = tmp->dequeue();

    std::string name = "";
    std::string perm = "";
    std::string size = "";
    std::string owner = "";
    std::string created = "";
    std::string mod = "";

    name = node->filename;
    for (int i = 0; i < 4; i++) perm += node->permission[i] * sizeof(char);
    size = std::to_string(node->size);
    owner = std::to_string(node->owner);

    if (node->active) {
      created = std::to_string(node->creation_time.count());
      mod = std::to_string(node->last_modified_time.count());
    } else {
      created = "0";
      mod = "0";
      owner = " ";
    }

    pad(name, 9, ' ');
    pad(perm, 5, ' ');
    pad(size, 5, ' ');
    pad(owner, 15, ' ');
    pad(created, 14, ' ');
    pad(mod, 13, ' ');

    str += " " + name + " | " + perm + " | " + size + " | " + owner + " | " + created +
      " | " + mod + "\n";
    ++ctr;

  } while (!tmp->empty());

  if (ctr == 0) return "\n There are no valid INODE structures!";
  return str;
}

std::string UFS::disk_contents() {
  std::string str = "";
  std::fstream disk("./disk/disk.txt", std::ios::in);

  disk.seekg(0, std::ios::end);
  str.reserve(disk.tellg());
  disk.seekg(0, std::ios::beg);
  str.assign(
    (std::istreambuf_iterator< char >(disk)), std::istreambuf_iterator< char >());

  return str;
}