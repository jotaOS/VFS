#ifndef ABSTRACT_HPP
#define ABSTRACT_HPP

// These are some abstractions of the ideas of file, directory, and such
// They're general to all filesystems, and if some doesn't support it, the
//   corresponding fields must be made up. For instance, users in ISO9660.

#include <string>

typedef size_t Inode;

struct File {
	// Which mountpoint?

	// Name is ommited

	Inode parent = 0;
	Inode inode = 0;
	size_t size = 0;

	bool isDirectory = false;

	// More will come in the future
	// Who created it, who modified it last. Timestamps.
};

#endif
