#include "TestMemoryPool.h"
#include "base/MemoryPool.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>


using namespace grid;


void TestMemoryPool::DoTest() {
    std::cout << "============= start MemoryPool test ===============" << std::endl;

    std::srand(std::time(0));
    MemoryPool::Instance().Cleanup();
    
    
    {
        const std::size_t size = 10;
        
        auto ptr1_0 = MemoryPool::Instance().Alloc(size);
        assert(MemoryPool::Instance().GetSize() == size);
        
        MemoryPool::Instance().Dealloc(ptr1_0);
        assert(MemoryPool::Instance().GetSize() == size);
    
        auto ptr1_1 = MemoryPool::Instance().Alloc(size - 1);
        assert(MemoryPool::Instance().GetSize() == size);
        assert(ptr1_0 == ptr1_1);

        MemoryPool::Instance().Dealloc(ptr1_1);
        
        auto ptr1_2 = MemoryPool::Instance().Alloc(size);
        assert(MemoryPool::Instance().GetSize() == size);
        assert(ptr1_0 == ptr1_2);
        
        MemoryPool::Instance().Dealloc(ptr1_2);
        
        //
        auto ptr2_0 = MemoryPool::Instance().Alloc(size * 2);
        assert(MemoryPool::Instance().GetSize() == size * 3);
        MemoryPool::Instance().Dealloc(ptr2_0);

        auto ptr2_1 = MemoryPool::Instance().Alloc(size * 2 - 1);
        assert(MemoryPool::Instance().GetSize() == size * 3);
        assert(ptr2_0 == ptr2_1);

        MemoryPool::Instance().Dealloc(ptr2_1);

        auto ptr2_2 = MemoryPool::Instance().Alloc(size * 2);
        assert(MemoryPool::Instance().GetSize() == size * 3);
        assert(ptr2_0 == ptr2_2);
        
        MemoryPool::Instance().Dealloc(ptr2_2);
        
        //
        auto ptr1_3 = MemoryPool::Instance().Alloc(size);
        assert(MemoryPool::Instance().GetSize() == size * 3);
        assert(ptr1_0 == ptr1_3);
        
        MemoryPool::Instance().Dealloc(ptr1_3);
        
        auto ptr1_4 = MemoryPool::Instance().Alloc(size - 1);
        assert(MemoryPool::Instance().GetSize() == size * 3);
        assert(ptr1_0 == ptr1_4);
        
        MemoryPool::Instance().Dealloc(ptr1_4);
        
        //
        MemoryPool::Instance().Cleanup();
        assert(MemoryPool::Instance().GetSize() == 0);
    }

    {
        const std::size_t size = 10;
        const int loopCount = 10;
        std::list<void*> blocks;

        for (int i = 1; i <= loopCount; ++i) {
            auto ptr = MemoryPool::Instance().Alloc(size);
            assert(MemoryPool::Instance().GetSize() == size * i);
            blocks.push_back(ptr);
        }

        for (auto& block : blocks) {
            MemoryPool::Instance().Dealloc(block);
        }
        blocks.clear();

        //
        for (int i = 1; i <= 1000; ++i) {
            auto ptr = MemoryPool::Instance().Alloc(std::rand() % (size + 1));
            assert(MemoryPool::Instance().GetSize() == size * loopCount);
            MemoryPool::Instance().Dealloc(ptr);
        }
        
        //
        MemoryPool::Instance().Cleanup();
        assert(MemoryPool::Instance().GetSize() == 0);
    }
    
    std::cout << "============= complete MemoryPool test ===============" << std::endl;
}

