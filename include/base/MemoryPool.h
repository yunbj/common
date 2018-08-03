#pragma once


#include <cstddef>
#include <map>
#include <mutex>


namespace grid {
    
    class MemoryPool {
    private:
        MemoryPool() = default;
        
        void _IncreaseSize(std::size_t n);

        static void* _GetOriginalAddress(void* ptr);
        
        static void* _GetUserAddress(void* ptr);
        
    public://noncopyable
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;
        MemoryPool& operator=(MemoryPool&&) = delete;
        
    public:
        static MemoryPool& Instance() {
            static MemoryPool inst;
            return inst;
        }
        
        ~MemoryPool();
        
        void* Alloc(std::size_t n);
        
        void Dealloc(void* ptr);
        
        uint64_t GetSize() const;
        
        void Cleanup();
        
    private:
        static const uint32_t kHeaderLength = sizeof(std::size_t);
        
    private:
        mutable std::mutex _mtx;
        std::multimap<std::size_t, uint8_t*> _blocks;
        uint64_t _totalSize;
    };//class MemoryPool
}//namespace grid
