#ifndef VFS_HPP
#define VFS_HPP

#include <common.hpp>
#include <string>
#include <pair>
#include <fss/abstract.hpp>
#include <unordered_map>
#include <fs>

std::pair<Mountpoint, File> find(const std::string& path);

typedef std::unordered_map<std::string, size_t> FileList;
bool list(Mountpoint, size_t, FileList&);
std::pair<uint8_t*, size_t> marshalledList(Mountpoint, Inode);

bool read(Mountpoint mp, Inode i, uint8_t* data, size_t start, size_t sz);
size_t write(Mountpoint mp, Inode i, uint8_t* data, size_t start, size_t sz);

size_t mkdir(Mountpoint mp, Inode i, std::string& name);
size_t mkfile(Mountpoint mp, Inode i, std::string& name);

bool addACL(Mountpoint mp, Inode i, size_t uid, std::ACLEntry entry);
std::ACL getACL(Mountpoint mp, Inode i);
std::ACL getEACL(const std::string& path);
std::pair<uint8_t*, size_t> marshalledACL(const std::ACL&);

#endif
