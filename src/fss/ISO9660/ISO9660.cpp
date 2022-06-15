#include "ISO9660.hpp"
#include <cstdio>
#include <shared_memory>
#include <userspace/ISO9660.hpp>

ISO9660::ISO9660(std::UUID uuid, bool bootstrapping) {
	if(!bootstrapping) {
		std::printf("[VFS] Oops\n");
		while(true);
	} else {
		pid = std::resolve("_bsISO");
		if(!pid) {
			std::printf("[VFS] Bootstrapping ISO9660 is not running.\n");
			std::exit(75); // Random number
		}
	}

	// Set up
	if(!std::rpc(pid, std::ISO9660::SETUP, uuid.a, uuid.b))
		std::printf("[VFS] Could not set up ISO9660.\n");

	// Shared memory
	smid = std::smMake();
	buffer = (uint8_t*)std::smMap(smid);
	std::smAllow(smid, pid);
	if(!std::rpc(pid, std::ISO9660::CONNECT, smid))
		std::printf("[VFS] Could not connect with ISO9660.\n");

	root = std::rpc(pid, std::ISO9660::GET_ROOT);
	if(!root)
		std::printf("[VFS] Could not find root.\n");
}
