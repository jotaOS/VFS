#include "ISO9660.hpp"
#include <userspace/ISO9660.hpp>
#include <cstdio>
#include <shared_memory>

bool ISO9660::read(Inode inode, uint8_t* data, size_t page, size_t n) {
	std::SMID smid = std::smMake(n);
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	bool ret = std::rpc(pid, std::ISO9660::READ, smid, inode, page, n);
	if(ret)
		memcpy(data, buffer, n * PAGE_SIZE);

	std::munmap(buffer, n);
	std::smDrop(smid);
	return ret;
}
