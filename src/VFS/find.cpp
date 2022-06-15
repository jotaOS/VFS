#include <common.hpp>
#include <unordered_set>
#include <fss/ISO9660/ISO9660.hpp>
#include "VFS.hpp"

std::pair<Mountpoint, File> find(const std::string& origpath) {
	std::string path = simplify(origpath);

	// Look through all mountpoints, find best match in a greedy-like way
	std::unordered_set<std::string> candidates;
	for(auto const& x : mounts)
		candidates.add(x.f);

	size_t counter = 0;
	while(candidates.size() != 1) {
		std::unordered_set<std::string> nextCandidates;
		for(auto const& x : candidates) {
			if(counter < x.size() && path[counter] == x[counter]) {
				// Looking good so far
				nextCandidates.add(x);
			}
		}

		candidates = std::move(nextCandidates);
		++counter; // Another character checked
	}

	std::string mpPath = *(candidates.begin());
	Mountpoint mp = mounts[mpPath];

	// Now that we got the mountpoint, what's the inode?
	// Remove mpPath from path
	std::string fsPath;
	for(size_t i=mpPath.size(); i<path.size(); ++i)
		fsPath += path[i];

	// And that's the path, handle it over to the filesystem routine
	File file;
	switch(fstypes[mp]) {
	case FSTypes::ISO9660:
		file = ((ISO9660*)(mountpoints[mp]))->find(fsPath);
		break;
	}

	return {mp, file};
}
