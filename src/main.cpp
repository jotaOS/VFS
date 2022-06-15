#include <cstdio>
#include <common.hpp>
#include <fss/ISO9660/ISO9660.hpp>

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

	if(devices.size() != 1) {
		std::printf("[VFS] Only one device supported for now :(\n");
		std::exit(99);
	}

	// Make an ISO9660 instance of the device
	ISO9660* iso = new ISO9660(devices[0], BOOTSTRAPPING);
	// That's my first mountpoint
	mountpoints.push_back(iso);
	size_t isomp = 0; // Mountpoint key of the CD
	fstypes.push_back(FSTypes::ISO9660);
	mounts["/"] = isomp;

	// That's it
	std::halt();
}
