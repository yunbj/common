#include <cstring>
#include "base/MemoryPool.h"


using namespace grid;


MemoryPool::~MemoryPool()
{
    this->cleanup();
}

void* MemoryPool::alloc(std::size_t n)
{
    if (n == 0)
    {
        n = 1;
    }
    
    std::lock_guard<std::mutex> guard(_mtx);
    
    uint8_t* ptr = nullptr;

    auto it = _blocks.lower_bound(n);
    if (it == _blocks.end())
    {
        ptr = new uint8_t[n + kHeaderLength];
        
        this->_increaseSize(n);
        
        //write header
        std::memcpy(ptr, &n, kHeaderLength);
    }
    else
    {
        ptr = it->second;
        _blocks.erase(it);
    }
    
    return _getUserAddress(ptr);
}

void MemoryPool::dealloc(void* ptr)
{
    if (!ptr)
    {
        return;
    }
    
    auto addr = static_cast<uint8_t*>(_getOriginalAddress(ptr));
    
    //read header
    std::size_t n = 0;
    std::memcpy(&n, addr, kHeaderLength);
    
    std::lock_guard<std::mutex> guard(_mtx);
    _blocks.insert(std::make_pair(n, addr));
}

uint64_t MemoryPool::getSize() const
{
    std::lock_guard<std::mutex> guard(_mtx);

    return _totalSize;
}

void MemoryPool::cleanup()
{
    std::lock_guard<std::mutex> guard(_mtx);

    for (auto& elem : _blocks)
    {
        delete[] elem.second;
    }
    
    _blocks.clear();
    _totalSize = 0;
}

void MemoryPool::_increaseSize(std::size_t n)
{
    _totalSize += n;
}

void* MemoryPool::_getOriginalAddress(void* ptr)
{
    return static_cast<uint8_t*>(ptr) - kHeaderLength;
}

void* MemoryPool::_getUserAddress(void* ptr)
{
    return static_cast<uint8_t*>(ptr) + kHeaderLength;
}
