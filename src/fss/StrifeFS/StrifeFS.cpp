#include "StrifeFS.hpp"
#include <cstdio>
#include <shared_memory>
#include <userspace/StrifeFS.hpp>

StrifeFS::StrifeFS(std::UUID uuid, bool bootstrapping, bool format) {
	if(!bootstrapping) {
		std::printf("[VFS] Oops (StrifeFS)\n");
		while(true);
	} else {
		pid = std::resolve("StrifeFS");
		if(!pid) {
			std::printf("[VFS] Bootstrapping StrifeFS is not running.\n");
			std::exit(76); // Random number
		}
	}

	// Set up
	if(!std::rpc(pid, std::StrifeFS::SETUP, uuid.a, uuid.b, format)) {
		std::printf("[VFS] Could not set up StrifeFS.\n");
		std::exit(77);
	}
}
