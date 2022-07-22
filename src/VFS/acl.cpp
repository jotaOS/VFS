#include "VFS.hpp"
#include <fss/StrifeFS/StrifeFS.hpp>
#include <userspace/VFS.hpp>

std::ACL getACL(Mountpoint mp, Inode i) {
	switch(fstypes[mp]) {
	case FSTypes::STRIFEFS:
		return ((StrifeFS*)(mountpoints[mp]))->getACL(i);
	}

	return {};
}

std::pair<uint8_t*, size_t> marshalledACL(const std::ACL& acl) {
	size_t sz = acl.size() * 2 * sizeof(uint64_t);
	uint8_t* buffer = new uint8_t[sz];

	uint64_t* ptr = (uint64_t*)buffer;
	for(auto const& x : acl) {
		*(ptr++) = x.f;
		*(ptr++) = x.s.raw;
	}

	return {buffer, sz};
}
