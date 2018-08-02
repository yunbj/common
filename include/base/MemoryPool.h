#pragma once


#include <map>
#include <mutex>


namespace grid
{
    class MemoryPool
    {
    private:
        MemoryPool() = default;
        
        void _increaseSize(std::size_t n);

        static void* _getOriginalAddress(void* ptr);
        
        static void* _getUserAddress(void* ptr);
        
    public://noncopyable
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;
        MemoryPool& operator=(MemoryPool&&) = delete;
        
    public:
        static MemoryPool& instance()
        {
            static MemoryPool inst;
            return inst;
        }
        
        ~MemoryPool();
        
        void* alloc(std::size_t n);
        
        void dealloc(void* ptr);
        
        uint64_t getSize() const;
        
        void cleanup();
        
    private:
        static const uint32_t kHeaderLength = sizeof(std::size_t);
        
    private:
        mutable std::mutex _mtx;
        std::multimap<std::size_t, uint8_t*> _blocks;
        uint64_t _totalSize;
    };//class MemoryPool
}//namespace grid
