#include "VFS.hpp"
#include <fss/StrifeFS/StrifeFS.hpp>
#include <userspace/VFS.hpp>

size_t write(Mountpoint mp, Inode i, uint8_t* data, size_t start, size_t sz) {
	switch(fstypes[mp]) {
	case FSTypes::STRIFEFS:
		if(((StrifeFS*)(mountpoints[mp]))->write(i, start, data, sz))
			return std::VFS::OK;
		else
			return std::VFS::ERROR_READING;
	}

	return std::VFS::READ_ONLY_FS;
}
