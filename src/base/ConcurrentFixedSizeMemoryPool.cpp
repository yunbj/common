#include "base/ConcurrentFixedSizeMemoryPool.h"
#include <cstring>
#include <cassert>


using namespace grid;

//using concurrent_unordered_map

ConcurrentFixedSizeMemoryPool::~ConcurrentFixedSizeMemoryPool() {
    this->Cleanup();
}

void ConcurrentFixedSizeMemoryPool::Cleanup() {
    for (auto& elem : _pools) {
        uint8_t* ptr = nullptr;
        while (elem.second.try_pop(ptr)) {
            delete[] ptr;
        }
    }
    
    _pools.clear();
    _totalSize = 0;
}

void* ConcurrentFixedSizeMemoryPool::Alloc(std::size_t n) {
    if (n == 0) {
        n = 1;
    }
    
    //find or create pool
    Pool_t& pool = _pools[n];
    
    //get or create memory
    uint8_t* ptr = nullptr;
    if (!pool.try_pop(ptr)) {
        ptr = new uint8_t[n + kHeaderLength];
        
        this->_IncreaseSize(n);
        
        //write header
        std::memcpy(ptr, &n, kHeaderLength);
    }
    
    return _GetUserAddress(ptr);
}

void ConcurrentFixedSizeMemoryPool::Dealloc(void* ptr) {
    if (!ptr) {
        return;
    }
    
    auto addr = static_cast<uint8_t*>(_GetOriginalAddress(ptr));
    
    //read header
    std::size_t n = 0;
    std::memcpy(&n, addr, kHeaderLength);
    
    auto it = _pools.find(n);
    if (it != _pools.end()) {
        it->second.push(addr);
    } else {
        assert(false);
    }
}

uint64_t ConcurrentFixedSizeMemoryPool::GetSize() const {
    return _totalSize;
}

std::size_t ConcurrentFixedSizeMemoryPool::GetPoolCount() const {
    return _pools.size();
}

std::size_t ConcurrentFixedSizeMemoryPool::GetAvailCount(std::size_t n) const {
    auto it = _pools.find(n);
    if (it != _pools.end()) {
        return it->second.unsafe_size();
    } else {
        return 0;
    }
}

void ConcurrentFixedSizeMemoryPool::_IncreaseSize(std::size_t n) {
    _totalSize += n;
}

void* ConcurrentFixedSizeMemoryPool::_GetOriginalAddress(void* ptr) {
    return static_cast<uint8_t*>(ptr) - kHeaderLength;
}

void* ConcurrentFixedSizeMemoryPool::_GetUserAddress(void* ptr) {
    return static_cast<uint8_t*>(ptr) + kHeaderLength;
}


//using concurrent_hash_map
/**
ConcurrentFixedSizeMemoryPool::~ConcurrentFixedSizeMemoryPool()
{
    for (auto& elem : _pools) {
        uint8_t* ptr = nullptr;
        while (elem.second.try_pop(ptr)) {
            delete[] ptr;
        }
    }
    
    _pools.clear();
    _totalSize = 0;
}

void* ConcurrentFixedSizeMemoryPool::Alloc(std::size_t n)
{
    assert(n > 0);

    //find or create pool
    Pool_t* pool = nullptr;
    {
        PoolAccessor accessor;
        _pools.insert(accessor, n);
        pool = &accessor->second;
    }
    
    //get or create memory
    uint8_t* ptr = nullptr;
    if (!pool->try_pop(ptr)) {
        ptr = new uint8_t[n + kHeaderLength];
        
        this->_IncreaseSize(n);
        
        //write header
        std::memcpy(ptr, &n, kHeaderLength);
    }
    
    return _GetUserAddress(ptr);
}

void ConcurrentFixedSizeMemoryPool::Dealloc(void* ptr)
{
    if (!ptr) {
        return;
    }
    
    auto addr = static_cast<uint8_t*>(_GetOriginalAddress(ptr));
    
    //read header
    std::size_t n = 0;
    std::memcpy(&n, addr, kHeaderLength);
    
    //find pool
    Pool_t* pool = nullptr;
    {
        PoolAccessor accessor;
        if (_pools.find(accessor, n)) {
            pool = &accessor->second;
        }
    }
    
    if (pool) {
        pool->push(addr);
    } else {
        assert(false);
    }
}

uint64_t ConcurrentFixedSizeMemoryPool::GetSize() const {
    return _totalSize;
}

std::size_t ConcurrentFixedSizeMemoryPool::GetPoolCount() const {
    return _pools.size();
}

std::size_t ConcurrentFixedSizeMemoryPool::GetAvailCount(std::size_t n) const {
    Pool_t* pool = nullptr;
    {
        PoolAccessor accessor;
        if (_pools.find(accessor, n)) {
            pool = &accessor->second;
        }
    }

    if (pool) {
        return pool->unsafe_size();
    } else {
        return 0;
    }
}

void ConcurrentFixedSizeMemoryPool::_IncreaseSize(std::size_t n) {
    _totalSize += n;
}

void* ConcurrentFixedSizeMemoryPool::_GetOriginalAddress(void* ptr) {
    return static_cast<uint8_t*>(ptr) - kHeaderLength;
}

void* ConcurrentFixedSizeMemoryPool::_GetUserAddress(void* ptr) {
    return static_cast<uint8_t*>(ptr) + kHeaderLength;
}
*/

