#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"

size_t StrifeFS::read(Inode inode, uint8_t* data, size_t page) {
	if(!std::rpc(pid, std::StrifeFS::GET_INODE, inode))
		return 0;

	auto* i = (SStructs::Inode*)buffer;
	size_t start = page * PAGE_SIZE;
	if(start >= i->size)
		return 0;

	size_t end = (page+1) * PAGE_SIZE;
	size_t size = std::min(end, i->size) - start;
	if(!size)
		return 0;

	memcpy(data, buffer, size);
	memset(data+size, 0, PAGE_SIZE - size);
	return size;
}
