#ifndef VFS_HPP
#define VFS_HPP

#include <common.hpp>
#include <string>
#include <pair>
#include <fss/abstract.hpp>
#include <unordered_map>

std::string simplify(const std::string& path);
std::pair<Mountpoint, File> find(const std::string& path);

typedef std::unordered_map<std::string, size_t> FileList;
bool list(Mountpoint, size_t, FileList&);
std::pair<uint8_t*, size_t> marshalledList(Mountpoint, Inode);

bool read(Mountpoint mp, Inode i, uint8_t* data, size_t page);

#endif
