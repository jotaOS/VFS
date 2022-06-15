#include "ISO9660.hpp"
#include <userspace/ISO9660.hpp>
#include <cstdio>

size_t ISO9660::read(Inode inode, uint8_t* data, size_t page) {
	size_t ret = std::rpc(pid, std::ISO9660::READ, inode, page);
	memcpy(data, buffer, ret);
	return ret;
}
