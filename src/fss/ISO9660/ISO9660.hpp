#ifndef ISO9660_HPP
#define ISO9660_HPP

#include "../abstract.hpp"
#include <unordered_map>

#define BOOTSTRAPPING true

class ISO9660 {
private:
	struct Flags {
		enum {
			HIDDEN,
			DIRECTORY,
			ASSOCIATED,
			EXTENDEDATTRS,
			OWNERSHIP,
			RESERVED0,
			RESERVED1,
		};
	};

	std::PID pid = 0;
	Inode root = 0;

public:
	ISO9660() = default;
	ISO9660(std::UUID uuid, bool bootstrapping=false);

	inline Inode getRoot() const { return root; }
	std::unordered_map<std::string, File> list(Inode inode);
	bool read(Inode inode, uint8_t* data, size_t page, size_t n);

	File find(const std::string& path);
};

#endif
