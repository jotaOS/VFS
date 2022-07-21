#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"
#include <shared_memory>

bool StrifeFS::read(Inode inode, size_t start, uint8_t* data, size_t sz) {
	size_t npages = NPAGES(sz);

	std::SMID smid = std::smMake(npages);
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	auto ret = std::rpc(pid, std::StrifeFS::READ, smid, inode, start, sz);
	if(ret)
		memcpy(data, buffer, sz);

	std::munmap(buffer, npages);
	std::smDrop(smid);
	return ret;
}
