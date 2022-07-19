#include <common.hpp>
#include <unordered_set>
#include <fss/ISO9660/ISO9660.hpp>
#include <fss/StrifeFS/StrifeFS.hpp>
#include "VFS.hpp"
#include <set>
#include <fs>

std::pair<Mountpoint, File> find(const std::string& origpath) {
	std::string path = std::simplifyPath(origpath);
	if(!path.size())
		return {0, {}};

	// Look through all mountpoints, find best match
	std::string mpPath;
	Mountpoint mp = 0;
	size_t highestScore = 0;
	std::set<std::pair<size_t, Mountpoint>> mps;
	for(auto const& x : mounts) {
		size_t score = 0;
		while(score < path.size() && score < x.f.size()) {
			if(path[score] == x.f[score])
				++score;
			else
				break;
		}

		if(score > highestScore && score == x.f.size()) {
			mpPath = x.f;
			mp = x.s;
			highestScore = score;
		}
	}

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
	case FSTypes::STRIFEFS:
		file = ((StrifeFS*)(mountpoints[mp]))->find(fsPath);
		break;
	}

	return {mp, file};
}
