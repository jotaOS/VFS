#include <cstdio>
#include <userspace/block.hpp>
#include <common.hpp>
#include <shared_memory>

std::unordered_map<size_t, std::vector<std::UUID>> probe() {
	std::PID block = std::resolve("block");
	if(!block) {
		std::printf("[VFS] Could not resolve for block.\n");
		std::exit(1);
	}

	std::SMID smid = std::smMake();
	uint64_t* buffer = (uint64_t*)std::smMap(smid);
	std::smAllow(smid, block);
	if(!std::rpc(block, std::block::CONNECT, smid)) {
		std::printf("[VFS] Could not share memory with block.\n");
		std::exit(2);
	}

	std::unordered_map<size_t, std::vector<std::UUID>> ret;

	size_t page=0;
	while(true) {
		size_t n = std::rpc(block,
							std::block::LIST_DEVICES,
							page);

		if(n == 0)
			break; // That's all

		while(n--) {
			std::UUID uuid;
			uuid.a = *(buffer++);
			uuid.b = *(buffer++);

			auto type = *(buffer++);
			ret[type].push_back(uuid);
		}

		++page;
	}

	// TODO: free smid, disconnect maybe?

	return ret;
}
