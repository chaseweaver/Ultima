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
}

/*
 * UFS::~UFS()
 * Default deconstructor.
 */
UFS::~UFS() {}

/*
 * UFS::format()
 * Formats the disk contents with the initialization char.
 */
void UFS::format() {
  std::fstream disk("./disk/disk.txt", std::ios::out);
  for (int i = 0; i < fs_block_size * fs_number_of_blocks; i++) {
    disk.seekp(i, std::ios::beg);
    disk.put(initilization_char);
  }
  init_inodes();
  disk.close();
}

/*
 * UFS::enough_inodes_available(int)
 * Returns true if enough empty blocks are available, false otherwise.
 */
bool UFS::enough_inodes_available(int num_of_blocks) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int counter = 0;
  do {
    INODE* node = tmp->dequeue();
    if (!node->active) ++counter;
    if (counter == num_of_blocks) return true;
  } while (!tmp->empty());

  return false;
}

/*
 * UFS::change_permission(int, std::string, int, char[4])
 * Changes the permission of a file if file OWNER.
 */
int UFS::change_permission(int file_handle, std::string name, const char perm[4]) {
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

  return success == true ? 1 : -1;
}

/*
 * UFS::delete_file(std::string, int, char[4])
 * "Deletes" a file by placing '$' inside of the file's contents.
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
      std::fstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
      for (int i = node->block_id * fs_block_size;
           i < node->block_id * fs_block_size + fs_block_size;
           i++) {
        disk.seekp(i, std::ios::beg);
        disk.put('$');
      }

      mcb->ui->write_refresh(LOG_WINDOW,
        " Del. file handle #" + std::to_string(node->file_id) + " @ location " +
          std::to_string(node->block_id * fs_block_size) + "\n");

      node->filename = "----------------";
      node->active = false;
      node->current_write = 0;
      node->current_read = 0;
      node->file_id = 0;
      node->size = 0;
      node->owner = 0;
      node->status = "EMPTY";
      node->creation_time = std::chrono::milliseconds::zero();
      node->last_modified_time = std::chrono::milliseconds::zero();
      for (int i = 0; i < 4; i++) node->permission[i] = '-';

      disk.flush();
      disk.close();

      success = true;
    }
  } while (--size != 0);

  ufs_sema->signal();

  write_inodes();
  return success == true ? 1 : -1;
}

/*
 * UFS::create_file(std::string, int, const char[4])
 * Initializes an INODE struct (>= 1 && <= 4) with default properties.
 * On success, returns a unique file handle, -1 otherwise.
 */
int UFS::create_file(std::string name, int file_size, const char perm[4]) {
  if (file_size > 4 * fs_block_size) return -1;

  int blocks_needed = file_size / fs_block_size + (file_size % fs_block_size != 0);

  if (!enough_inodes_available(blocks_needed)) return -1;

  std::chrono::milliseconds t =
    duration_cast< milliseconds >(system_clock::now().time_since_epoch());
  int handle = next_handle();
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);

  do {
    if (blocks_needed == 0) break;

    INODE* node = tmp->dequeue();
    if (node->active) continue;

    node->filename = name;
    for (int i = 0; i < 4; i++) node->permission[i] = perm[i];

    node->owner = pthread_self();
    node->file_id = handle;
    node->active = true;
    node->creation_time = t;
    node->last_modified_time = t;
    node->current_read = 0;
    node->current_write = 0;

    --blocks_needed;
    write_inodes();

  } while (!tmp->empty());
  return handle;
}

/*
 * UFS::open(int, std::string, char)
 * Returns 1 if a file can be opened successfully from the caller, -1 otherwise.
 */
int UFS::open(int file_handle, std::string name, char mode) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();

  do {
    INODE* node = tmp->dequeue();
    if (node->owner == pthread_self() ||
      (node->filename == name) &&
        (node->permission[2] == mode || node->permission[3] == mode)) {
      if (mode == 'R')
        node->status = "READ";
      else if (mode == 'W')
        node->status = "WRITE";
      node->file_id == file_handle;
      return next_unique_f_handle();
    }
  } while (--size != 0);
  return -1;
}

/*
 * UFS::return_inode(int)
 * Returns the INODE with requested ID.
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
 * UFS::return_nonactive_inode(int)
 * Returns the first nonactive INODE with the requested ID.
 */
UFS::INODE* UFS::return_nonactive_inode(int file_handle) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);

  do {
    INODE* node = tmp->dequeue();
    if (node->file_id == file_handle && node->current_write == 0) return node;
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
  set_write(file_handle);
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
 * Validation check to ensure enough INODEs with registered IDs are available.
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
  ufs_sema->wait();

  if (str.length() < 128) {
    char ch[str.length()];

    INODE* node = return_nonactive_inode(file_handle);
    if (node == nullptr) return -1;

    for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

    int offset = node->block_id * fs_block_size;
    std::fstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
    set_write(file_handle);
    for (int i = 0; i < strlen(ch) && i < str.length(); i++) {
      disk.seekp(offset + node->current_write++, std::ios::beg);
      disk.put(ch[i]);
      ++node->size;
    }

    node->last_modified_time = node->last_modified_time =
      duration_cast< milliseconds >(system_clock::now().time_since_epoch());

    write_inodes();
    disk.close();
    ufs_sema->signal();

  } else {
    char ch[fs_block_size];

    INODE* node = return_nonactive_inode(file_handle);
    if (node == nullptr) return -1;

    for (int i = 0; i < sizeof(ch); i++) ch[i] = str[i];

    int offset = node->block_id * fs_block_size;
    std::fstream disk("./disk/disk.txt", std::ios::in | std::ios::out);
    set_write(file_handle);

    for (int i = 0; i < strlen(ch) - 1 && i < str.length(); i++) {
      disk.seekp(offset + node->current_write++, std::ios::beg);
      disk.put(ch[i]);
      ++node->size;
    }


    node->last_modified_time = node->last_modified_time =
      duration_cast< milliseconds >(system_clock::now().time_since_epoch());

    write_inodes();
    disk.close();

    ufs_sema->signal();

    if (str.length() > fs_block_size)
      write_string(file_handle,
        str.substr(str.length() / fs_block_size +
            (str.length() % fs_block_size != 0) * fs_block_size - 1,
          str.length()));
  }
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

  ufs_sema->wait();

  std::ifstream disk("./disk/disk.txt", std::ios::in);
  set_read(file_handle);

  disk.seekg(node->block_id * fs_block_size + node->current_read++, std::ios::beg);
  char c;
  disk.get(c);
  ch = &c;

  disk.close();

  ufs_sema->signal();
  return 1;
}

/*
 * UFS::next_handle()
 * Increments and returns a file handle.
 */
int UFS::next_handle() { return ++next_file_handle; }

/*
 * UFS::next_unique_f_handle()
 * Increments and returns a unique file handle.
 */
int UFS::next_unique_f_handle() { return ++next_unique_file_handle; }

/*
 * UFS::init_inodes()
 * Initializes INODE structures with default values.
 */
void UFS::init_inodes() {
  ufs_sema->wait();
  while (!nodes.empty()) nodes.dequeue();

  for (int i = 0; i < fs_number_of_blocks; i++) {
    INODE* node = new INODE;

    node->filename = "----------------";
    node->owner = 0;
    node->block_id = i;
    node->size = 0;
    node->file_id = 0;
    node->status = "EMPTY";
    node->creation_time = std::chrono::milliseconds::zero();
    node->last_modified_time = std::chrono::milliseconds::zero();

    for (int j = 0; j < 4; j++) node->permission[j] = '-';
    node->active = false;
    node->current_read = 0;
    node->current_write = 0;

    nodes.enqueue(node);
  }

  ufs_sema->signal();
}

/*
 * UFS::write_inodes(bool)
 * Writes INODE structures to a disk.
 */
void UFS::write_inodes() {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  int size = tmp->size();
  std::string str = "";

  do {
    INODE* node = tmp->dequeue();
    str += build_inode(node);
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
 * Returns the amount of INODEs on a disk.
 */
int UFS::amount_of_inodes() { return nodes.size(); }

/*
 * UFS::build_inode(INODE*)
 * Converts an INODE structure into a string.
 */
std::string UFS::build_inode(INODE* node) {
  std::string str = "";

  str += node->filename + ' ';
  str += std::to_string((unsigned long int)node->owner) + ' ';

  str += (node->block_id >= 10) ? std::to_string(node->block_id) + ' '
                                : std::to_string(node->block_id) + "  ";
  str += std::to_string(node->size) + ' ';

  for (int i = 0; i < 4; i++) str += node->permission[i] * sizeof(char);

  str += ' ';
  str += std::to_string(node->active) + ' ';
  str += std::to_string(node->current_read) + ' ';
  str += std::to_string(node->current_write) + ' ';
  str += std::to_string(node->file_id) + ' ';
  str += std::to_string(node->creation_time.count()) + ' ';
  str += std::to_string(node->last_modified_time.count()) + '\n';
  return str;
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
  std::string status_tile = "Status";
  std::string owner_title = "Owner";
  std::string created_time_tile = "Time Created";
  std::string last_mod_tile = "Last Modified";
  std::string pbreak = " ";
  pad(pbreak, 91, '-');

  pad(name_title, 16, ' ');
  pad(perm_title, 4, ' ');
  pad(size_title, 4, ' ');
  pad(status_tile, 3, ' ');
  pad(owner_title, 15, ' ');
  pad(created_time_tile, 13, ' ');
  pad(last_mod_tile, 12, ' ');

  str += name_title + " | " + perm_title + " | " + size_title + " | " + status_tile +
    " | " + owner_title + " | " + created_time_tile + " | " + last_mod_tile + "\n" +
    pbreak + "\n";

  do {
    INODE* node = tmp->dequeue();

    std::string name = node->filename;
    std::string perm = "";
    for (int i = 0; i < 4; i++) perm += node->permission[i] * sizeof(char);
    std::string size = std::to_string(node->size);

    std::string owner =
      node->active ? std::to_string((unsigned long int)node->owner) : "0";
    std::string created = std::to_string(node->creation_time.count());
    std::string mod = std::to_string(node->last_modified_time.count());
    std::string status = node->status;

    pad(name, 16, ' ');
    pad(perm, 5, ' ');
    pad(size, 4, ' ');
    pad(status, 6, ' ');
    pad(owner, 15, ' ');
    pad(created, 13, ' ');
    pad(mod, 12, ' ');

    str += " " + name + " | " + perm + " | " + size + " | " + status + " | " + owner +
      " | " + created + " | " + mod + "\n";
    ++ctr;

  } while (!tmp->empty());

  if (ctr == 0) return "\n There are no valid INODE structures!";
  return str;
}

/*
 * UFS::disk_contents()
 * Lists the contents of the disk.
 */
std::string UFS::disk_contents() {
  std::string str = "";
  std::fstream disk("./disk/disk.txt", std::ios::in);

  disk.seekg(0, std::ios::end);
  str.reserve(disk.tellg());
  disk.seekg(0, std::ios::beg);
  str.assign(
    (std::istreambuf_iterator< char >(disk)), std::istreambuf_iterator< char >());

  disk.close();
  return str;
}

/*
 * UFS::inode_contents()
 * Lists the contents of the INODE disk.
 */
std::string UFS::inode_contents() {
  std::string str = "";
  std::fstream disk("./disk/inode_fs.txt", std::ios::in);

  disk.seekg(0, std::ios::end);
  str.reserve(disk.tellg());
  disk.seekg(0, std::ios::beg);
  str.assign(
    (std::istreambuf_iterator< char >(disk)), std::istreambuf_iterator< char >());

  disk.close();
  return str;
}

/*
 * UFS::set_status(int, std::string)
 * Sets the INODE status to a string.
 * Returns 1 if successful, -1 otherwise.
 */
int UFS::set_status(int file_id, std::string status) {
  Queue< INODE* >* tmp = new Queue< INODE* >(nodes);
  bool success = false;

  do {
    INODE* node = tmp->dequeue();
    if (node->file_id == file_id) {
      node->status = status;
      success = true;
    }
  } while (!tmp->empty());
  return success == true ? 1 : -1;
}

/*
 * UFS::close(int)
 * Sets the INODE status to CLOSED.
 * Returns 1 if successful, -1 otherwise.
 */
int UFS::close(int file_id) { return set_status(file_id, "CLOSED"); }

/*
 * UFS::set_read(int)
 * Sets the INODE status to READ.
 * Returns 1 if successful, -1 otherwise.
 */
int UFS::set_read(int file_id) { return set_status(file_id, "READ"); }

/*
 * UFS::set_write(int)
 * Sets the INODE status to WRITE.
 * Returns 1 if successful, -1 otherwise.
 */
int UFS::set_write(int file_id) { return set_status(file_id, "WRITE"); }