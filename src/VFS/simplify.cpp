#include <common.hpp>
#include "VFS.hpp"

std::string simplify(const std::string& path) {
	std::vector<std::string> parts;
	for(auto const& part : path.split('/')) {
		if(part == ".") {
			// Ignore
		} else if(part == "..") {
			// Take last one back
			if(parts.size())
				parts.pop_back();
		} else {
			// LGTM
			parts.push_back(part);
		}
	}

	std::string ret;
	for(auto const& x : parts)
		ret += x + "/";
	ret.pop_back(); // Remove last slash

	return ret;
}
