#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"

Inode StrifeFS::makeFile(Inode parent, std::string& name) {
	if(name.size() >= PAGE_SIZE)
		return 0;
	memcpy(buffer, name.c_str(), name.size());
	return std::rpc(pid, std::StrifeFS::MAKE_FILE, parent, name.size());
}

Inode StrifeFS::makeDir(Inode parent, std::string& name) {
	if(name.size() >= PAGE_SIZE)
		return 0;
	memcpy(buffer, name.c_str(), name.size());
	return std::rpc(pid, std::StrifeFS::MAKE_DIR, parent, name.size());
}
