#include <cstdio>
#include <common.hpp>
#include <fss/ISO9660/ISO9660.hpp>
#include <fss/StrifeFS/StrifeFS.hpp>
#include <userspace/block.hpp>

std::vector<void*> mountpoints;
std::vector<size_t> fstypes;
std::unordered_map<std::string, size_t> mounts;

extern "C" void _start(bool fromISO) {
	IGNORE(fromISO); // Assuming 'yes' for now

	// Find devices
	auto devices = probe();
	if(!devices.size()) {
		std::printf("[VFS] No devices found!\n");
		std::exit(3);
	}

	auto& ramblocks = devices[std::block::DeviceTypes::RAMBLOCK];
	if(fromISO && !ramblocks.size()) {
		std::printf("[VFS] Booting from CD, but no ramblock\n");
		std::exit(97);
	} else if(fromISO && ramblocks.size() > 1) {
		std::printf("[VFS] I need only one ramblock\n");
		std::exit(96);
	}
	auto ramblock = ramblocks[0];

	// StrifeFS instance of the ramblock
	StrifeFS* root = new StrifeFS(ramblock, true, DO_FORMAT);
	mountpoints.push_back(root);
	size_t rootmp = 0; // Mountpoint key
	fstypes.push_back(FSTypes::STRIFEFS);
	mounts["/"] = rootmp;

	// CD will be mounted in /cd/
	std::string strcd = "cd";
	root->makeDir(1, strcd);

	auto& atapis = devices[std::block::DeviceTypes::AHCIATAPI];
	if(fromISO && !atapis.size()) {
		std::printf("[VFS] Booting from CD, but no ATAPI device\n");
		std::exit(99);
	} else if(fromISO && atapis.size() > 1) {
		std::printf("[VFS] I need only one ATAPI device :(\n");
		std::exit(98);
	}
	auto atapi = atapis[0];


	// Make an ISO9660 instance of the device
	ISO9660* iso = new ISO9660(atapi, BOOTSTRAPPING);
	mountpoints.push_back(iso);
	size_t isomp = 1; // Mountpoint key of the CD
	fstypes.push_back(FSTypes::ISO9660);
	mounts["/cd/"] = isomp;

	// That's it
	publish();
	std::halt();
}
