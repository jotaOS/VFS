#ifndef STRIFEFS_HPP
#define STRIFEFS_HPP

#include "../abstract.hpp"
#include <unordered_map>

#define DO_FORMAT true

class StrifeFS {
private:
	std::PID pid = 0;
	static const Inode root = 1;

public:
	StrifeFS() = default;
	StrifeFS(std::UUID uuid, bool bootstrapping=false, bool format=false);

	static inline Inode getRoot() { return root; }

	std::unordered_map<std::string, File> list(Inode inode);
	size_t read(Inode inode, uint8_t* data, size_t page);
	bool write(Inode inode, size_t start, uint8_t* data, size_t sz);

	File find(const std::string& path);

	Inode makeFile(Inode parent, std::string& name);
	Inode makeDir(Inode parent, std::string& name);
};

#endif
