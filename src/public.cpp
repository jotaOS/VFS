#include <common.hpp>
#include <unordered_map>
#include <string>
#include <mutex>
#include <VFS/VFS.hpp>
#include <shared_memory>
#include <rpc>
#include <userspace/VFS.hpp>

typedef std::pair<Mountpoint, File> Mpf; // Mountpoint + Inode

std::unordered_map<std::PID, Mpf> selected;
std::mutex selectedLock;
size_t select(std::PID client, std::SMID smid, size_t sz) {
	if(sz >= PAGE_SIZE)
		return std::VFS::SELECT_CONNECT_ERROR;

	auto link = std::sm::link(client, smid);
	size_t npages = link.s;
	if(!npages)
		return std::VFS::SELECT_CONNECT_ERROR;
	uint8_t* buffer = link.f;

	if(sz >= npages * PAGE_SIZE) {
		std::sm::unlink(smid);
		return std::VFS::SELECT_CONNECT_ERROR;
	}
	buffer[sz] = 0;
	std::string name((char*)buffer);
	std::sm::unlink(smid);

	auto found = find(name);
	auto mp = found.f;
	auto i = found.s.inode;

	// TODO: check permissions of parent (can read?)
	// return 2;

	if(!i)
		return std::VFS::SELECT_NOT_FOUND;

	selectedLock.acquire();
	selected[client] = {mp, found.s};
	selectedLock.release();
	return std::VFS::SELECT_OK;
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

	auto ret = marshalledList(sel.f, sel.s.inode);
	if(!ret.f)
		return false;

	return (ret.s + PAGE_SIZE - 1) / PAGE_SIZE;
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

bool pubRead(std::PID client, std::SMID smid, size_t page) {
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

	auto ret = read(sel.f, sel.s.inode, buffer, page);
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
	info.size = sel.s.size;
	info.isDirectory = sel.s.isDirectory;
	memcpy(buffer, &info, sizeof(info));
	std::sm::unlink(smid);
	return true;
}

void publish() {
	std::exportProcedure((void*)select, 2);
	std::exportProcedure((void*)pubListSize, 0);
	std::exportProcedure((void*)pubList, 1);
	std::exportProcedure((void*)pubRead, 2);
	std::exportProcedure((void*)pubInfo, 1);
	// Write, makeFile, makeDir
	std::enableRPC();
	std::publish("VFS");
}
