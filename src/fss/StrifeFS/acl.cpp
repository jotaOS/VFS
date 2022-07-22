#include "StrifeFS.hpp"

std::ACL StrifeFS::getACL(Inode inode) {
	// Get the ACL inode
	size_t acli = readInode(inode).acl;
	if(!acli)
		return {}; // No ACL

	std::ACL ret;

	// How much is there to read?
	size_t size = readInode(acli).size;
	uint8_t* buffer = new uint8_t[size];
	read(acli, 0, buffer, size);

	size_t n = size / sizeof(SStructs::ACLEntry);
	auto* ptr = (SStructs::ACLEntry*)buffer;
	while(n--) {
		std::ACLEntry entry;
		entry = ptr->e;
		entry.reserved = 0;
		ret[ptr->id] = entry;
		++ptr;
	}

	delete [] buffer;
	return ret;
}
