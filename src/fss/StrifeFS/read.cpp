#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"
#include <shared_memory>

size_t StrifeFS::read(Inode inode, uint8_t* data, size_t page) {
	std::SMID smid = std::smMake();
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	if(!std::rpc(pid, std::StrifeFS::GET_INODE, inode)) {
		std::munmap(buffer);
		std::smDrop(smid);
		return 0;
	}

	/*auto* i = (SStructs::Inode*)buffer;
	size_t start = page * PAGE_SIZE;

	if(start >= i->size) {
		std::munmap(buffer);
		std::smDrop(smid);
		return 0;
	}

	size_t end = (page+1) * PAGE_SIZE;
	size_t size = std::min(end, i->size) - start;
	if(!size) {
		std::munmap(buffer);
		std::smDrop(smid);
		return 0;
		}*/

	//if(!std::rpc(pid, std::StrifeFS::READ

	/*memcpy(data, buffer, size);
	memset(data+size, 0, PAGE_SIZE - size);*/

	IGNORE(data); IGNORE(page);
	std::printf("TODO!\n"); while(true);
	//return size;
	return 0;
}
