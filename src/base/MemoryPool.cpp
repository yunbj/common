#include <cstring>
#include "base/MemoryPool.h"


using namespace grid;


MemoryPool::~MemoryPool() {
    this->Cleanup();
}

void* MemoryPool::Alloc(std::size_t n) {
    if (n == 0) {
        n = 1;
    }
    
    std::lock_guard<std::mutex> guard(_mtx);
    
    uint8_t* ptr = nullptr;

    auto it = _blocks.lower_bound(n);
    if (it == _blocks.end()) {
        ptr = new uint8_t[n + kHeaderLength];
        
        this->_IncreaseSize(n);
        
        //write header
        std::memcpy(ptr, &n, kHeaderLength);
    }
    else {
        ptr = it->second;
        _blocks.erase(it);
    }
    
    return _GetUserAddress(ptr);
}

void MemoryPool::Dealloc(void* ptr) {
    if (!ptr) {
        return;
    }
    
    auto addr = static_cast<uint8_t*>(_GetOriginalAddress(ptr));
    
    //read header
    std::size_t n = 0;
    std::memcpy(&n, addr, kHeaderLength);
    
    std::lock_guard<std::mutex> guard(_mtx);
    _blocks.insert(std::make_pair(n, addr));
}

uint64_t MemoryPool::GetSize() const {
    std::lock_guard<std::mutex> guard(_mtx);

    return _totalSize;
}

void MemoryPool::Cleanup() {
    std::lock_guard<std::mutex> guard(_mtx);

    for (auto& elem : _blocks) {
        delete[] elem.second;
    }
    
    _blocks.clear();
    _totalSize = 0;
}

void MemoryPool::_IncreaseSize(std::size_t n) {
    _totalSize += n;
}

void* MemoryPool::_GetOriginalAddress(void* ptr) {
    return static_cast<uint8_t*>(ptr) - kHeaderLength;
}

void* MemoryPool::_GetUserAddress(void* ptr) {
    return static_cast<uint8_t*>(ptr) + kHeaderLength;
}
