#include "StrifeFS.hpp"
#include <userspace/StrifeFS.hpp>
#include <cstdio>
#include "structures.hpp"
#include <shared_memory>

std::unordered_map<std::string, File> StrifeFS::list(Inode inode) {
	std::SMID smid = std::smMake();
	uint8_t* buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	if(!std::rpc(pid, std::StrifeFS::GET_INODE, inode)) {
		std::munmap(buffer);
		std::smDrop(smid);
		return {};
	}

	auto* theInode = (SStructs::Inode*)buffer;
	size_t sz = theInode->size;
	size_t npages = NPAGES(sz);

	std::munmap(buffer);
	std::smDrop(smid);

	smid = std::smMake(npages);
	buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);

	if(!std::rpc(pid, std::StrifeFS::READ, inode, 0, sz)) {
		std::munmap(buffer, npages);
		std::smDrop(smid);
		return {};
	}

	std::unordered_map<std::string, File> ret;

	uint8_t* cur = buffer;
	size_t remaining = sz;
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

		std::SMID ismid = std::smMake();
		uint8_t* ibuffer = (uint8_t*)std::smMap(ismid);
		std::smAllow(ismid, pid);
		if(!std::rpc(pid, std::StrifeFS::GET_INODE, ismid, i)) {
			std::munmap(ibuffer);
			std::smDrop(ismid);
			continue;
		}

		auto* thing = (SStructs::Inode*)ibuffer;

		File f;
		f.parent = inode;
		f.inode = i;
		f.size = thing->size;
		if(thing->type == SStructs::Inode::Types::DIRECTORY)
			f.isDirectory = true;

		ret[name] = f;

		std::munmap(ibuffer);
		std::smDrop(ismid);
	}

	std::munmap(buffer, npages);
	std::smDrop(smid);
	return ret;
}
