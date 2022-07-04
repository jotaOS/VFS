#ifndef STRIFEFS_STRUCTURES_HPP
#define STRIFEFS_STRUCTURES_HPP

// Copypasted from the StrifeFS one
namespace SStructs {
	typedef uint64_t LBA;
	typedef uint64_t Timestamp;
	typedef uint64_t Inodei;
	typedef uint64_t Blocki;
	typedef uint64_t UID;
	typedef uint64_t GID;

	const size_t DIRECT_BLOCKS = 12;

	struct Inode {
		// Metadata
		uint64_t size;
		Timestamp ctime, mtime, atime;
		uint64_t links;
		uint64_t nblocks;

		struct Types {
			enum {
				REGULAR,
				DIRECTORY,
				ACL,
				LINK,
			};
		};
		size_t type;

		// ACL
		Blocki acl;

		// Data
		Blocki block0[DIRECT_BLOCKS];
		Blocki iblock[3];
	} __attribute__((packed));

	struct ACLEntry {
		uint64_t allow : 1;
		uint64_t isUser : 1;
		uint64_t read : 1;
		uint64_t write : 1;
		uint64_t execute : 1;
		uint64_t reserved : 59;

		uint64_t id;
	} __attribute__((packed));
}

#endif
