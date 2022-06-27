#include "ISO9660.hpp"
#include <userspace/ISO9660.hpp>
#include <cstdio>

// This in the marshalled return of list
struct MarshalledFile {
	Inode inode;
	uint64_t date;
	uint32_t size;
	uint8_t flags;
	uint8_t namesz = 0;
} __attribute__((packed));

std::unordered_map<std::string, File> ISO9660::list(Inode inode) {
	// Get how many pages, and the first one
	size_t npages = std::rpc(pid, std::ISO9660::LIST, inode, 0);
	if(npages == 0)
		return {}; // Sad

	uint8_t* aux = new uint8_t[npages * PAGE_SIZE];
	memcpy(aux, buffer, PAGE_SIZE);
	uint8_t* cur = aux + PAGE_SIZE;

	// Copy the rest
	for(size_t i=1; i<npages; ++i) {
		std::rpc(pid, std::ISO9660::LIST, inode, i);
		memcpy(cur, buffer, PAGE_SIZE);
		cur += PAGE_SIZE;
	}
	// Got it
	uint8_t* absoluteLimit = aux + npages * PAGE_SIZE;

	std::unordered_map<std::string, File> ret;

	cur = aux;
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

	return ret;
}
