#include "VFS.hpp"
#include <fss/StrifeFS/StrifeFS.hpp>
#include <userspace/VFS.hpp>

size_t mkdir(Mountpoint mp, Inode i, std::string& name) {
	switch(fstypes[mp]) {
	case FSTypes::STRIFEFS:
		if(((StrifeFS*)(mountpoints[mp]))->makeDir(i, name))
			return std::VFS::OK;
		else
			return std::VFS::ERROR_READING; // This for instance, idk
	}

	return std::VFS::READ_ONLY_FS;
}

size_t mkfile(Mountpoint mp, Inode i, std::string& name) {
	switch(fstypes[mp]) {
	case FSTypes::STRIFEFS:
		if(((StrifeFS*)(mountpoints[mp]))->makeFile(i, name))
			return std::VFS::OK;
		else
			return std::VFS::ERROR_READING;
	}

	return std::VFS::READ_ONLY_FS;
}
