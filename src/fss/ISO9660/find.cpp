#include "ISO9660.hpp"

File ISO9660::find(const std::string& path) {
	Inode parent = root;
	File current;

	auto splitted = path.split('/');
	auto filename = splitted.back();
	splitted.pop_back();

	// Check the directories
	for(auto const& x : splitted) {
		current = list(parent)[x];
		if(!current.inode)
			return {}; // Doesn't exist!
		if(!current.isDirectory)
			return {}; // It's not a directory!

		parent = current.inode;
	}

	// Directories are done. Is the file there?
	current = list(parent)[filename];
	if(!current.inode)
		return {};
	return current;
}
