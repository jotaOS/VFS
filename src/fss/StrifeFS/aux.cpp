#include "StrifeFS.hpp"
#include <shared_memory>
#include <rpc>
#include <userspace/StrifeFS.hpp>

SStructs::Inode StrifeFS::readInode(Inode inode) {
	std::SMID smid = std::smMake();
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	if(!std::rpc(pid, std::StrifeFS::GET_INODE, smid, inode)) {
		std::munmap(buffer);
		std::smDrop(smid);
		return {};
	}

	auto ret = *(SStructs::Inode*)buffer;

	std::munmap(buffer);
	std::smDrop(smid);

	return ret;
}
