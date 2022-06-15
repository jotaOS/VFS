#ifndef VFS_HPP
#define VFS_HPP

#include <string>
#include <pair>
#include <fss/abstract.hpp>

std::string simplify(const std::string& path);
std::pair<Mountpoint, File> find(const std::string& path);

#endif
