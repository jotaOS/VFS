#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"

bool StrifeFS::write(Inode inode, size_t start, uint8_t* data, size_t sz) {
	if(sz > PAGE_SIZE)
		return false;

	memcpy(buffer, data, sz);
	return std::rpc(pid, std::StrifeFS::WRITE, inode, start, sz);
}
