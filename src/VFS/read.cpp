#include "VFS.hpp"
#include <fss/ISO9660/ISO9660.hpp>
#include <fss/StrifeFS/StrifeFS.hpp>

bool read(Mountpoint mp, Inode i, uint8_t* data, size_t page) {
	switch(fstypes[mp]) {
	case FSTypes::ISO9660:
		return ((ISO9660*)(mountpoints[mp]))->read(i, data, page);
	case FSTypes::STRIFEFS:
		return ((StrifeFS*)(mountpoints[mp]))->read(i, data, page);
	}

	return false;
}
