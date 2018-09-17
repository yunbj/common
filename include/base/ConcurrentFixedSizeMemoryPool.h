#pragma once


#include <cstddef>
#include <thread>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_queue.h>
#include <tbb/atomic.h>
#include "base/Gcd.h"


namespace grid
{
    class ConcurrentFixedSizeMemoryPool
    {
    private:
        ConcurrentFixedSizeMemoryPool();

        void _Dump();

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

    public:
        static std::size_t AlignSize(std::size_t n);

        static const uint32_t kHeaderLength = sizeof(std::size_t);

        static const std::size_t kUnit = 10*1024;//10KB;

    private:
        using Pool_t = tbb::concurrent_queue<uint8_t*>;
        using PoolValue = std::pair<tbb::atomic<std::size_t>, Pool_t>;//<allocated count, pool>
        
        tbb::concurrent_unordered_map<std::size_t, PoolValue> _pools;
        tbb::atomic<uint64_t> _totalSize = 0;

        grid::Gcd _gcd;
        uint32_t _timerId;
    };//class ConcurrentFixedSizeMemoryPool
}//namespace grid
