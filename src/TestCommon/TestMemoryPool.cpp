#include "TestMemoryPool.h"
#include "base/MemoryPool.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <list>


using namespace grid;


void TestMemoryPool::DoTest()
{
    std::cout << "============= start MemoryPool test ===============" << std::endl;

    std::srand(std::time(0));
    MemoryPool::instance().cleanup();
    
    
    {
        const std::size_t size = 10;
        
        auto ptr1_0 = MemoryPool::instance().alloc(size);
        assert(MemoryPool::instance().getSize() == size);
        
        MemoryPool::instance().dealloc(ptr1_0);
        assert(MemoryPool::instance().getSize() == size);
    
        auto ptr1_1 = MemoryPool::instance().alloc(size - 1);
        assert(MemoryPool::instance().getSize() == size);
        assert(ptr1_0 == ptr1_1);

        MemoryPool::instance().dealloc(ptr1_1);
        
        auto ptr1_2 = MemoryPool::instance().alloc(size);
        assert(MemoryPool::instance().getSize() == size);
        assert(ptr1_0 == ptr1_2);
        
        MemoryPool::instance().dealloc(ptr1_2);
        
        //
        auto ptr2_0 = MemoryPool::instance().alloc(size * 2);
        assert(MemoryPool::instance().getSize() == size * 3);
        MemoryPool::instance().dealloc(ptr2_0);

        auto ptr2_1 = MemoryPool::instance().alloc(size * 2 - 1);
        assert(MemoryPool::instance().getSize() == size * 3);
        assert(ptr2_0 == ptr2_1);

        MemoryPool::instance().dealloc(ptr2_1);

        auto ptr2_2 = MemoryPool::instance().alloc(size * 2);
        assert(MemoryPool::instance().getSize() == size * 3);
        assert(ptr2_0 == ptr2_2);
        
        MemoryPool::instance().dealloc(ptr2_2);
        
        //
        auto ptr1_3 = MemoryPool::instance().alloc(size);
        assert(MemoryPool::instance().getSize() == size * 3);
        assert(ptr1_0 == ptr1_3);
        
        MemoryPool::instance().dealloc(ptr1_3);
        
        auto ptr1_4 = MemoryPool::instance().alloc(size - 1);
        assert(MemoryPool::instance().getSize() == size * 3);
        assert(ptr1_0 == ptr1_4);
        
        MemoryPool::instance().dealloc(ptr1_4);
        
        //
        MemoryPool::instance().cleanup();
        assert(MemoryPool::instance().getSize() == 0);
    }

    {
        const std::size_t size = 10;
        const int loopCount = 10;
        std::list<void*> blocks;

        for (int i = 1; i <= loopCount; ++i)
        {
            auto ptr = MemoryPool::instance().alloc(size);
            assert(MemoryPool::instance().getSize() == size * i);
            blocks.push_back(ptr);
        }

        for (auto& block : blocks)
        {
            MemoryPool::instance().dealloc(block);
        }
        blocks.clear();

        //
        for (int i = 1; i <= 1000; ++i)
        {
            auto ptr = MemoryPool::instance().alloc(std::rand() % (size + 1));
            assert(MemoryPool::instance().getSize() == size * loopCount);
            MemoryPool::instance().dealloc(ptr);
        }
        
        //
        MemoryPool::instance().cleanup();
        assert(MemoryPool::instance().getSize() == 0);
    }
    
    std::cout << "============= complete MemoryPool test ===============" << std::endl;
}

