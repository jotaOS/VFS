#include "VFS.hpp"
#include <fss/ISO9660/ISO9660.hpp>
#include <fss/StrifeFS/StrifeFS.hpp>

bool list(Mountpoint mp, Inode i, FileList& ret) {
	std::unordered_map<std::string, File> l;

	switch(fstypes[mp]) {
	case FSTypes::ISO9660:
		l = ((ISO9660*)(mountpoints[mp]))->list(i);
		break;
	case FSTypes::STRIFEFS:
		l = ((StrifeFS*)(mountpoints[mp]))->list(i);
		break;
	}

	if(!l.size())
		return false;
	for(auto const& x : l)
		ret[x.f] = x.s.inode;

	return true;
}

std::pair<uint8_t*, size_t> marshalledList(Mountpoint mp, Inode i) {
	FileList l;
	if(!list(mp, i, l))
		return {nullptr, 0};

	size_t sz = l.size() * 2 * sizeof(uint64_t);
	for(auto const& x : l)
		sz += x.f.size();

	uint8_t* ptr = new uint8_t[sz];
	uint8_t* cur = ptr;
	for(auto& x : l) {
		*(uint64_t*)cur = x.s;
		cur += sizeof(uint64_t);
		*(uint64_t*)cur = x.f.size();
		cur += sizeof(uint64_t);
		memcpy(cur, x.f.c_str(), x.f.size());
		cur += x.f.size();
	}

	return {ptr, sz};
}
