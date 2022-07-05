#include <common.hpp>
#include <unordered_map>
#include <string>
#include <mutex>
#include <VFS/VFS.hpp>
#include <shared_memory>
#include <rpc>
#include <userspace/VFS.hpp>

bool connect(std::PID client, std::SMID smid) {
	return std::sm::connect(client, smid);
}

typedef std::pair<Mountpoint, Inode> Mpi; // Mountpoint + Inode

std::unordered_map<std::PID, Mpi> selected;
std::mutex selectedLock;
size_t select(std::PID client, size_t sz) {
	if(sz >= PAGE_SIZE)
		return std::VFS::SELECT_CONNECT_ERROR;

	uint8_t* data = std::sm::get(client);
	if(!data)
		return std::VFS::SELECT_CONNECT_ERROR;
	data[sz] = 0;
	std::string name((char*)data);

	auto found = find(name);
	auto mp = found.f;
	auto i = found.s.inode;

	// TODO: check permissions of parent (can read?)
	// return 2;

	if(!i)
		return std::VFS::SELECT_NOT_FOUND;

	selectedLock.acquire();
	selected[client] = {mp, i};
	selectedLock.release();
	return std::VFS::SELECT_OK;
}

bool pubList(std::PID client, size_t page) {
	uint8_t* data = std::sm::get(client);
	if(!data)
		return false;

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	FileList files;
	if(!list(sel.f, sel.s, files))
		return false;

	auto ret = marshalledList(sel.f, sel.s);
	if(!ret.f)
		return false;

	size_t npages = (ret.s + PAGE_SIZE - 1) / PAGE_SIZE;
	if(page >= npages) {
		delete [] ret.f;
		return false;
	}

	if(page == npages-1)
		memset(data, 0, PAGE_SIZE);

	size_t start = page * PAGE_SIZE;
	if(start < ret.s)
		memcpy(data, ret.f+start, std::min(ret.s - start, (size_t)PAGE_SIZE));

	delete [] ret.f;
	return true;
}

bool pubRead(std::PID client, size_t page) {
	uint8_t* data = std::sm::get(client);
	if(!data)
		return false;

	selectedLock.acquire();
	if(!selected.has(client)) {
		selectedLock.release();
		return false;
	}
	auto sel = selected[client];
	selectedLock.release();

	// TODO: check permissions

	return read(sel.f, sel.s, data, page);
}

void publish() {
	std::exportProcedure((void*)connect, 1);
	std::exportProcedure((void*)select, 1);
	std::exportProcedure((void*)pubList, 1);
	std::exportProcedure((void*)pubRead, 1);
	// Write, makeFile, makeDir
	std::enableRPC();
	std::publish("VFS");
	std::halt();
}
