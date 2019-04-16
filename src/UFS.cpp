#include "../inc/UFS.h"

UFS::UFS(std::string filesystem_name, int num_blocks, int size_block, char init_char)
  : fs_name(filesystem_name), fs_number_of_blocks(num_blocks), fs_block_size(size_block),
    initilization_char(init_char) {
			file_system = new char[num_blocks * size_block];
  		std::fill_n(file_system, num_blocks * size_block, init_char);
		}