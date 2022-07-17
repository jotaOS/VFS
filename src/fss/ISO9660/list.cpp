#include "ISO9660.hpp"
#include <userspace/ISO9660.hpp>
#include <cstdio>
#include <shared_memory>

// This in the marshalled return of list
struct MarshalledFile {
	Inode inode;
	uint64_t date;
	uint32_t size;
	uint8_t flags;
	uint8_t namesz = 0;
} __attribute__((packed));

std::unordered_map<std::string, File> ISO9660::list(Inode inode) {
	// How many pages?
	size_t npages = std::rpc(pid, std::ISO9660::LIST_SIZE, inode);
	if(npages == 0)
		return {}; // Sad

	std::SMID smid = std::smMake(npages);
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	bool result = std::rpc(pid, std::ISO9660::LIST, smid, inode);
	if(!result) {
		std::munmap(buffer, npages);
		std::smDrop(smid);
		return {};
	}

	// Got it
	uint8_t* absoluteLimit = buffer + npages * PAGE_SIZE;
	std::unordered_map<std::string, File> ret;

	uint8_t* cur = buffer;
	while(cur < absoluteLimit) {
		auto* mf = (MarshalledFile*)cur;
		char* name = (char*)cur + sizeof(MarshalledFile);

		if(!mf->inode)
			break; // That was it

		File file;
		file.parent = inode;
		file.inode = mf->inode;
		file.size = mf->size;
		file.isDirectory = mf->flags & (1 << Flags::DIRECTORY);

		ret[name] = file;
		cur += sizeof(MarshalledFile);
		cur += mf->namesz + 1;
	}

	std::munmap(buffer, npages);
	std::smDrop(smid);
	return ret;
}
