#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"
#include <shared_memory>

Inode StrifeFS::makeFile(Inode parent, std::string& name) {
	if(name.size() >= PAGE_SIZE)
		return 0;

	std::SMID smid = std::smMake();
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	memcpy(buffer, name.c_str(), name.size());
	auto ret = std::rpc(pid, std::StrifeFS::MAKE_FILE, smid, parent, name.size());

	std::munmap(buffer);
	std::smDrop(smid);
	return ret;
}

Inode StrifeFS::makeDir(Inode parent, std::string& name) {
	if(name.size() >= PAGE_SIZE)
		return 0;

	std::SMID smid = std::smMake();
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	memcpy(buffer, name.c_str(), name.size());
	auto ret = std::rpc(pid, std::StrifeFS::MAKE_DIR, parent, name.size());

	std::munmap(buffer);
	std::smDrop(smid);
	return ret;
}
