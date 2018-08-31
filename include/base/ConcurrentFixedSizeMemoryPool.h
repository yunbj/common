#pragma once


#include <cstddef>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_queue.h>
#include <tbb/atomic.h>


namespace grid
{
    class ConcurrentFixedSizeMemoryPool
    {
    private:
        ConcurrentFixedSizeMemoryPool() = default;
        
        void _IncreaseSize(std::size_t n);
        
        static void* _GetOriginalAddress(void* ptr);
        
        static void* _GetUserAddress(void* ptr);
        
    public://noncopyable
        ConcurrentFixedSizeMemoryPool(const ConcurrentFixedSizeMemoryPool&) = delete;
        ConcurrentFixedSizeMemoryPool(ConcurrentFixedSizeMemoryPool&&) = delete;
        ConcurrentFixedSizeMemoryPool& operator=(const ConcurrentFixedSizeMemoryPool&) = delete;
        ConcurrentFixedSizeMemoryPool& operator=(ConcurrentFixedSizeMemoryPool&&) = delete;
        
    public:
        static ConcurrentFixedSizeMemoryPool& Instance()
        {
            static ConcurrentFixedSizeMemoryPool inst;
            return inst;
        }
        
        ~ConcurrentFixedSizeMemoryPool();
        
        void* Alloc(std::size_t n);
        
        void Dealloc(void* ptr);
        
        uint64_t GetSize() const;
        
        std::size_t GetPoolCount() const;
        
        //thread-unsafe
        std::size_t GetAvailCount(std::size_t n) const;
        
        //thread-unsafe
        void Cleanup();
        
    private:
        static const uint32_t kHeaderLength = sizeof(std::size_t);
        
    private:
        using Pool_t = tbb::concurrent_queue<uint8_t*>;
        
        tbb::concurrent_unordered_map<std::size_t, Pool_t> _pools;
        tbb::atomic<uint64_t> _totalSize;
        
        //using PoolAccessor = decltype(_pools)::accessor;
    };//class ConcurrentFixedSizeMemoryPool
}//namespace grid
