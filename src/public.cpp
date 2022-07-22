#include <common.hpp>
#include <unordered_map>
#include <string>
#include <mutex>
#include <VFS/VFS.hpp>
#include <shared_memory>
#include <rpc>
#include <fs>

typedef std::pair<Mountpoint, File> Mpf; // Mountpoint + Inode

std::unordered_map<std::PID, Mpf> selected;
std::mutex selectedLock;
size_t select(std::PID client, std::SMID smid) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return std::VFS::CONNECT_ERROR;
	uint8_t* buffer = link.f;
	buffer[PAGE_SIZE-1] = 0;
	std::string name((char*)buffer);
	std::sm::unlink(smid);

	auto found = find(name);
	auto mp = found.f;
	auto i = found.s.inode;

	// TODO: check permissions of parent (can read?)
	// return 2;

	if(!i)
		return std::VFS::NOT_FOUND;

	selectedLock.acquire();
	selected[client] = {mp, found.s};
	selectedLock.release();
	return std::VFS::OK;
}

size_t pubListSize(std::PID client) {
	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	FileList files;
	if(!list(sel.f, sel.s.inode, files))
		return false;

	auto ms = marshalledList(sel.f, sel.s.inode);
	if(!ms.f)
		return false;
	auto ret = NPAGES(ms.s);

	delete [] ms.f;
	return ret;
}

bool pubList(std::PID client, std::SMID smid) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return false;
	uint8_t* buffer = link.f;

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		std::sm::unlink(smid);
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	FileList files;
	if(!list(sel.f, sel.s.inode, files)) {
		std::sm::unlink(smid);
		return false;
	}

	auto ret = marshalledList(sel.f, sel.s.inode);
	if(!ret.f) {
		std::sm::unlink(smid);
		return false;
	}

	memcpy(buffer, ret.f, std::min(ret.s, npages * PAGE_SIZE));

	delete [] ret.f;
	std::sm::unlink(smid);
	return true;
}

bool pubRead(std::PID client, std::SMID smid, size_t start, size_t sz) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return false;
	uint8_t* buffer = link.f;

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		std::sm::unlink(smid);
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	// TODO: check permissions

	auto ret = read(sel.f, sel.s.inode, buffer, start, sz);
	std::sm::unlink(smid);
	return ret;
}

bool pubInfo(std::PID client, std::SMID smid) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return false;
	uint8_t* buffer = link.f;

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		std::sm::unlink(smid);
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	// TODO: check permissions

	std::VFS::Info info;
	info.error = std::VFS::OK;
	info.size = sel.s.size;
	info.isDirectory = sel.s.isDirectory;
	memcpy(buffer, &info, sizeof(info));
	std::sm::unlink(smid);
	return true;
}

size_t pubMkdir(std::PID client, std::SMID smid) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return std::VFS::CONNECT_ERROR;
	uint8_t* buffer = link.f;
	buffer[PAGE_SIZE-1] = 0;
	std::string name((char*)buffer);
	std::sm::unlink(smid);

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	if(!sel.s.isDirectory)
		return std::VFS::NOT_A_DIRECTORY;

	return mkdir(sel.f, sel.s.inode, name);
	// TODO: check permissions
}

size_t pubMkfile(std::PID client, std::SMID smid) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return std::VFS::CONNECT_ERROR;
	uint8_t* buffer = link.f;
	buffer[PAGE_SIZE-1] = 0;
	std::string name((char*)buffer);
	std::sm::unlink(smid);

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	if(!sel.s.isDirectory)
		return std::VFS::NOT_A_DIRECTORY;

	return mkfile(sel.f, sel.s.inode, name);
	// TODO: check permissions
}

size_t pubWrite(std::PID client, std::SMID smid, size_t start, size_t sz) {
	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return std::VFS::CONNECT_ERROR;
	uint8_t* buffer = link.f;

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		std::sm::unlink(smid);
		return std::VFS::CONNECT_ERROR;
	}
	auto sel = selected[client];
	selectedLock.release();

	// TODO: check permissions

	if(npages < NPAGES(sz)) {
		std::sm::unlink(smid);
		return std::VFS::CONNECT_ERROR;
	}

	auto ret = write(sel.f, sel.s.inode, buffer, start, sz);
	std::sm::unlink(smid);
	return ret;
}

size_t pubGetACLSize(std::PID client) {
	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return 0;
	}
	auto sel = selected[client];
	selectedLock.release();

	auto acl = getACL(sel.f, sel.s.inode);
	auto ms = marshalledACL(acl);
	auto ret = ms.s;
	delete [] ms.f;
	return ret;
}

bool pubGetACL(std::PID client, std::SMID smid) {
	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return 0;
	}
	auto sel = selected[client];
	selectedLock.release();

	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return false;
	uint8_t* buffer = link.f;

	auto acl = getACL(sel.f, sel.s.inode);
	auto ms = marshalledACL(acl);
	if(npages < NPAGES(ms.s)) {
		delete [] ms.f;
		std::sm::unlink(smid);
		return false;
	}

	memcpy(buffer, ms.f, ms.s);
	delete [] ms.f;
	std::sm::unlink(smid);
	return true;
}

void publish() {
	std::exportProcedure((void*)select, 1);
	std::exportProcedure((void*)pubListSize, 0);
	std::exportProcedure((void*)pubList, 1);
	std::exportProcedure((void*)pubRead, 3);
	std::exportProcedure((void*)pubWrite, 3);
	std::exportProcedure((void*)pubInfo, 1);
	std::exportProcedure((void*)pubMkdir, 1);
	std::exportProcedure((void*)pubMkfile, 1);
	std::exportProcedure((void*)pubGetACLSize, 0);
	std::exportProcedure((void*)pubGetACL, 1);
	std::enableRPC();
	std::publish("VFS");
}
