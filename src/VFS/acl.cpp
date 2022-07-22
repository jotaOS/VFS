#include "VFS.hpp"
#include <fss/StrifeFS/StrifeFS.hpp>
#include <userspace/VFS.hpp>

bool addACL(Mountpoint mp, Inode i, size_t uid, std::ACLEntry entry) {
	switch(fstypes[mp]) {
	case FSTypes::STRIFEFS:
		return ((StrifeFS*)(mountpoints[mp]))->addACL(i, uid, entry);
	}

	return false;
}

std::ACL getACL(Mountpoint mp, Inode i) {
	switch(fstypes[mp]) {
	case FSTypes::STRIFEFS:
		return ((StrifeFS*)(mountpoints[mp]))->getACL(i);
	}

	return {};
}

std::ACL getEACL(const std::string& path) {
	std::ACL ret;

	auto parts = path.split('/');
	if(!parts[parts.size()-1].size())
		parts.pop_back();

	std::string stack = "/";
	for(size_t i=0; i<parts.size(); ++i) {
		stack += parts[i];
		if(i != parts.size()-1)
			stack += "/";
		stack = std::simplifyPath(stack);

		// We've got a path
		auto mf = find(stack);
		if(!mf.s.inode)
			return {}; // Not found!

		// Get its ACL
		auto acl = getACL(mf.f, mf.s.inode);
		// And combine
		for(auto entry : acl) {
			size_t id = entry.f;
			std::ACLEntry ent = entry.s;

			if(ent.allow)
				ret[id].raw |= ent.raw;
			else
				ret[id].raw &= ~ent.raw;

			ret[id].allow = true;
		}
	}

	return ret;
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
