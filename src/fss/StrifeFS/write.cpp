#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"
#include <shared_memory>

bool StrifeFS::write(Inode inode, size_t start, uint8_t* data, size_t sz) {
	size_t npages = (sz + PAGE_SIZE - 1) / PAGE_SIZE;

	std::SMID smid = std::smMake(npages);
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);
	memcpy(buffer, data, sz);

	auto ret = std::rpc(pid, std::StrifeFS::WRITE, inode, start, sz);

	std::munmap(buffer, npages);
	std::smDrop(smid);
	return ret;
}
