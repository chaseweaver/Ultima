#include "../inc/UFS.h"

UFS::UFS(std::string filesystem_name, int num_blocks, int size_block, char init_char)
  : fs_name(filesystem_name), fs_number_of_blocks(num_blocks), fs_block_size(size_block),
    initilization_char(init_char) {
  file_system = new char[num_blocks * size_block];
  std::fill_n(file_system, num_blocks * size_block, init_char);

  next_file_handle = 0;
  next_unique_file_handle = 0;

  std::fstream disk("./disk/disk.txt", std::ios::in);
    if(disk.fail()){
        //the file didnt already exist, so format a new disk
        format();
    }
  disk.close();

  //format();
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
  int size = tmp->size();

  do {
    INODE* node = tmp->dequeue();

    // Double check
    for (int i = 0; i < 4; i++)
      if (node->blocks[i] != 0) continue;

    // Fix at later
    for (int i = 0; i < 8; i++) node->filename[i] = name[i];

    node->owner = pthread_self();
    node->size = file_size;

    // Fix this later
    for (int i = 0; i < 4; i++) node->permission[i] = perm[i];

    // Probably fix later
    for (int i = 0; i < blocks_needed; i++)
      node->blocks[(node->starting_block / fs_block_size) - 1] = 0xFF;

    node->unique_file_handle = next_handle();
    return node->unique_file_handle;
  } while (--size != 0);
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
      return (node->unique_file_handle = file_handle);

  } while (--size != 0);

  return -1;
}

UFS::INODE* UFS::return_inode(int file_handle) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();

  do {
    INODE* node = tmp->dequeue();
    if (node->unique_file_handle == file_handle) return node;
  } while (--size != 0);

  return nullptr;
}

int UFS::write_char(int file_handle, char ch) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  std::fstream disk("./disk/disk.txt", std::ios::out);
  disk.seekp(node->starting_block * fs_block_size + node->current_write++);
  disk.put(ch);

  disk.close();

  return 1;
}

int UFS::write_string(int file_handle, std::string str) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  //char ch[str.length()];
  //for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

  std::ofstream disk("./disk/disk.txt", std::ios::in);
  disk.close();
  disk.clear();
  disk.open("./disk/disk.txt", std::ios::out | std::ios::in);
  

  int loc = node->starting_block * fs_block_size + node->current_write;
  int len = str.length();
  char thisMsg[len + 1];

  disk.seekp(loc);

  strcpy(thisMsg, str.c_str());

  //disk << str;

  disk.write(thisMsg, len);
  node->current_write+=len;

  disk.close();
  //disk.seekp(node->starting_block + node->current_write++);
  //disk.write(ch, strlen(ch));

  //TESING
  return node->current_write;
  //return 1;
}

// int UFS::write_char(int file_handle, std::string) {
// 	INODE* node = return_inode(file_handle);
// 	if (node == nullptr)
// 		return -1;

// 	std::fstream disk("./disk/disk.txt", std::ios::out);
// 	disk.seekp(node->starting_block + node->current_write++, std::ios::beg);
// 	disk.put(ch);

// 	disk.close();

// 	return 1;
// }

int UFS::read_char(int file_handle, char* ch) {
  INODE* node = return_inode(file_handle);
  if (node == nullptr) return -1;

  std::string name(node->filename);

  std::fstream disk("./disk/disk.txt", std::ios::out);
  disk.seekg(node->starting_block * fs_block_size + node->current_read++, std::ios::beg);
  char c;
  disk.get(c);
  ch = &c;

  // disk.close();

  return 1;
}

int UFS::next_handle() { return ++next_file_handle; }

//int UFS::next_unique_handle() { return ++next_unique_file_handle; }

void UFS::init_inodes() {
  for (int i = 0; i < fs_number_of_blocks; i++) {
    INODE* node = new INODE;

    std::fill_n(node->filename, 8, ' ');

    for (int j = 0; j < 4; j++) node->blocks[j] = 0;

    // node->owner_task_id = -1;
    node->starting_block = i;
    node->size = fs_block_size;

    // Fix this later
    for (int j = 0; j < 4; j++) node->permission[j] = '-';

    node->current_read = 0;
    node->current_write = 0;

    node->creation_time = node->last_modified_time =
      duration_cast< milliseconds >(system_clock::now().time_since_epoch());
    nodes.enqueue(node);
  }
}