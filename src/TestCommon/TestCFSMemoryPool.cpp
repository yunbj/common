#include "TestCFSMemoryPool.h"
#include "base/ConcurrentFixedSizeMemoryPool.h"
#include "base/Gcd.h"
#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <memory>
#include <numeric>
#include <atomic>
#include <thread>
#include <chrono>


void grid::TestCFSMemoryPool::DoTest() {
    std::cout << "============= start CFSMemroyPool test ===============" << std::endl;

    ConcurrentFixedSizeMemoryPool::Instance().Cleanup();
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetSize() == 0);
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() == 0);
    
    //init
    const uint32_t gcdCount = 100;
    
    std::list<std::pair<std::shared_ptr<grid::Gcd>, std::list<uint8_t*>>> gcds;
    for (int i = 0; i < gcdCount; ++i) {
        auto gcd = std::make_shared<grid::Gcd>();
        assert(gcd->Init() == 0);
        gcds.push_back(std::make_pair(gcd, std::list<uint8_t*>{}));
    }

    //
    const std::vector<uint32_t> memSizes{100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    const uint32_t allocCount = memSizes.size() * 1000;

    std::atomic<int> doneCount = 0;
    
    for (auto& elem : gcds) {
        auto gcd = elem.first;
        auto& list = elem.second;
        
        gcd->DispatchAsync([memSizes, allocCount, &doneCount, &list]() {
            int idx = 0;
            for (uint32_t i = 0; i < allocCount; ++i) {
                auto n = memSizes[idx++ % memSizes.size()];
                auto ptr = static_cast<uint8_t*>(ConcurrentFixedSizeMemoryPool::Instance().Alloc(n));
                assert(ptr);
                list.push_back(ptr);
                
                assert(ConcurrentFixedSizeMemoryPool::Instance().GetAvailCount(n) == 0);
            }
            ++doneCount;
        });
    }
    
    while (doneCount != gcdCount) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    //fini
    for (auto& elem : gcds) {
        auto gcd = elem.first;
        auto& list = elem.second;
        
        gcd->DispatchAsync([&list]() {
            for (auto ptr : list) {
                ConcurrentFixedSizeMemoryPool::Instance().Dealloc(ptr);
            }
            list.clear();
        });
    }

    for (auto& elem : gcds) {
        auto gcd = elem.first;
        gcd->Fini(true);
    }
    
    uint32_t totalSize = std::accumulate(memSizes.begin(), memSizes.end(), 0) * (allocCount / memSizes.size()) * gcdCount;
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetSize() == totalSize);
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() == memSizes.size());
    
    for (auto size : memSizes) {
        auto count = (allocCount / memSizes.size()) * gcdCount;
        assert(ConcurrentFixedSizeMemoryPool::Instance().GetAvailCount(size) == count);
    }
    
    std::cout << "============= complete CFSMemroyPool test ===============" << std::endl;
}
