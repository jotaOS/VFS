#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"

std::unordered_map<std::string, File> StrifeFS::list(Inode inode) {
	if(!std::rpc(pid, std::StrifeFS::GET_INODE, inode))
		return {};

	auto* theInode = (SStructs::Inode*)buffer;
	size_t sz = theInode->size;
	uint8_t* aux = new uint8_t[sz];

	// Start asking for pages
	size_t start = 0;
	size_t remaining = sz;
	uint8_t* cur = aux;
	while(remaining) {
		size_t copy = std::min(remaining, (size_t)PAGE_SIZE);
		if(!std::rpc(pid, std::StrifeFS::READ, inode, start, copy)) {
			delete [] aux;
			return {};
		}
		memcpy(cur, buffer, copy);
		start += copy; cur += copy; remaining -= copy;
	}

	std::unordered_map<std::string, File> ret;

	cur = aux;
	remaining = sz;
	while(remaining) {
		auto i = *(SStructs::Inodei*)cur;
		uint64_t namesz = *(uint64_t*)(cur + sizeof(SStructs::Inodei));
		cur += 2 * sizeof(uint64_t);
		remaining -= 2 * sizeof(uint64_t);

		char* tmp = new char[namesz + 1];
		memcpy(tmp, cur, namesz);
		tmp[namesz] = 0;
		std::string name(tmp);
		delete [] tmp;
		cur += namesz; remaining -= namesz;

		if(!std::rpc(pid, std::StrifeFS::GET_INODE, i))
			continue;

		auto* thing = (SStructs::Inode*)buffer;

		File f;
		f.parent = inode;
		f.inode = i;
		f.size = thing->size;
		if(thing->type == SStructs::Inode::Types::DIRECTORY)
			f.isDirectory = true;

		ret[name] = f;
	}

	delete [] aux;
	return ret;
}
