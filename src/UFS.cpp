#include "../inc/UFS.h"

UFS::UFS(std::string filesystem_name, int num_blocks, int size_block, char init_char)
  : fs_name(filesystem_name), fs_number_of_blocks(num_blocks), fs_block_size(size_block),
    initilization_char(init_char) {
  file_system = new char[num_blocks * size_block];
  std::fill_n(file_system, num_blocks * size_block, init_char);

  next_file_handle = 0;
  next_unique_file_handle = 0;

  format();
  init_inodes();
}

void UFS::format() {
  std::fstream disk("./disk/disk.txt", std::ios::out);
  for (int i = 0; i < fs_block_size * fs_number_of_blocks; i++) {
    disk.seekp(i, std::ios::beg);
    disk.put(initilization_char);
  }

  disk.close();
}

int UFS::create_file(char name[8], int file_size, char perm[4]) {
  if (file_size > 4 * fs_block_size) return -1;

  int blocks_needed = file_size / fs_block_size + (file_size % fs_block_size != 0);
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);

  do {
    INODE* node = tmp->dequeue();
    if (node->active) continue;

    for (int i = 0; i < 8; i++) node->filename[i] = name[i];
    for (int i = 0; i < 4; i++) node->permission[i] = perm[i];

    node->owner = pthread_self();
    node->size = file_size;
    node->id = next_handle();
    node->active = true;

    return node->id;
  } while (!tmp->empty());
  return -1;
}

int UFS::open(int file_handle, char name[8], char mode) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();

  do {
    INODE* node = tmp->dequeue();

    // SUPER FIX THIS
    if (node->owner == pthread_self() ||
      strcmp(node->filename, name) &&
        (node->permission[2] == mode || node->permission[3] == mode))
      return (node->id = file_handle);

  } while (--size != 0);
  return -1;
}

UFS::INODE* UFS::return_inode(int file_handle) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);

  do {
    INODE* node = tmp->dequeue();
    if (node->id == file_handle) return node;
  } while (!tmp->empty());

  return nullptr;
}

int UFS::write_char(int file_handle, char ch) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  std::ofstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
  disk.seekp(node->block_id * node->size + node->current_write++);
  disk.put(ch);

  disk.close();
  return 1;
}

int UFS::write_string(int file_handle, std::string str) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  char ch[str.length() + 1];
  for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  std::fstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
  int offset = node->block_id * node->size;
  for (int i = 0; i < strlen(ch); i++) {
    disk.seekp(offset + node->current_write++, std::ios::beg);
    disk.put(ch[i]);
  }

  disk.close();
  return 1;
}

int UFS::read_char(int file_handle, char* ch) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  std::string name(node->filename);

  std::fstream disk("./disk/disk.txt", std::ios::out);
  disk.seekg(node->block_id * fs_block_size + node->current_read++, std::ios::beg);
  char c;
  disk.get(c);
  ch = &c;

  disk.close();

  return 1;
}

int UFS::next_handle() { return ++next_file_handle; }

int UFS::next_unique_handle() { return ++next_unique_file_handle; }

void UFS::init_inodes() {
  for (int i = 0; i < fs_number_of_blocks; i++) {
    INODE* node = new INODE;

    std::fill_n(node->filename, 8, '-');
    node->owner = pthread_self();
    node->block_id = i;
    node->size = fs_block_size;

    for (int j = 0; j < 4; j++) node->permission[j] = '-';
    node->active = false;

    node->current_read = 0;
    node->current_write = 0;
    node->creation_time = node->last_modified_time =
      duration_cast< milliseconds >(system_clock::now().time_since_epoch());

    nodes.enqueue(node);
  }
}

void UFS::write_inodes() {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();
  std::string str = "";

  do { str += construct_inode(tmp->dequeue()); } while (--size != 0);
  char ch[str.length() + 1];
  for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  std::fstream disk("./disk/inode_fs.txt", std::ios::out | std::ios::trunc);
  disk.seekp(std::ios::beg);
  disk.write(ch, strlen(ch));

  disk.close();
}

int UFS::amount_of_inodes() { return nodes.size(); }

std::string UFS::construct_inode(INODE* node) {
  std::string str = "";
  for (int i = 0; i < 4; i++)
    str += std::bitset< 4 >(node->filename[i] * sizeof(char)).to_string();
  str += std::bitset< 20 >(node->owner).to_string();
  str += std::bitset< 8 >(node->block_id).to_string();
  str += std::bitset< 8 >(node->size).to_string();
  for (int i = 0; i < 4; i++)
    str += std::bitset< 4 >(node->permission[i] * sizeof(char)).to_string();
  str += std::bitset< 1 >(node->active).to_string();
  str += std::bitset< 8 >(node->current_read).to_string();
  str += std::bitset< 8 >(node->current_write).to_string();
  str += std::bitset< 8 >(node->id).to_string();
  str += std::bitset< 45 >(node->creation_time.count()).to_string();
  str += std::bitset< 45 >(node->last_modified_time.count()).to_string();
  str += '\n';
  return str;
}

std::string UFS::read_inodes() {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();
  std::string str = "";

  // do { str += construct_inode(tmp->dequeue()); } while (--size != 0);
  // char ch[str.length() + 1];
  // for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  // std::fstream disk("./disk/inode_fs.txt", std::ios::out);
  // disk.seekp(std::ios::beg);
  // disk.write(ch, strlen(ch));

  // disk.close();
}