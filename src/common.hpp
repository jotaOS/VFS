#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <unordered_map>
#include <string>

std::vector<std::UUID> probe();

struct FSTypes {
	enum {
		ISO9660,
	};
};

typedef size_t Mountpoint;

// Pointer to fs objects
extern std::vector<void*> mountpoints;
// Of what type is a mountpoint?
extern std::vector<size_t> fstypes;
// Resolving names to mountpoints
extern std::unordered_map<std::string, Mountpoint> mounts;

#endif
