#include "TestCFSMemoryPool.h"
#include "base/ConcurrentFixedSizeMemoryPool.h"
#include "base/Buffer.h"
#include "base/Gcd.h"
#include "tbb/atomic.h"
#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <memory>
#include <numeric>
#include <atomic>
#include <thread>
#include <chrono>
#include <cassert>
#include <future>
#include <mutex>

using namespace grid;


static void _testBasic() {
    std::cout << "* _testBasic() started" << std::endl;

    const auto kUnit = ConcurrentFixedSizeMemoryPool::kUnit;

    assert(ConcurrentFixedSizeMemoryPool::AlignSize(kUnit - 10) == kUnit);
    assert(ConcurrentFixedSizeMemoryPool::AlignSize(kUnit) == kUnit);
    assert(ConcurrentFixedSizeMemoryPool::AlignSize(kUnit + 10) == 2*kUnit);
    assert(ConcurrentFixedSizeMemoryPool::AlignSize(2*kUnit) == 2*kUnit);
    assert(ConcurrentFixedSizeMemoryPool::AlignSize(2*kUnit + 10) == 3*kUnit);

    ConcurrentFixedSizeMemoryPool::Instance().Cleanup();
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetSize() == 0);
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() == 0);

    const uint32_t size = 100;

    grid::Gcd allocer, deallocer;
    assert(allocer.Init() == 0);
    assert(deallocer.Init() == 0);

    for (int i = 0; i < 1000000; ++i) {
        allocer.DispatchAsync([&deallocer]() {
            auto ptr = ConcurrentFixedSizeMemoryPool::Instance().Alloc(size);
            deallocer.DispatchAsync([ptr]() {
                ConcurrentFixedSizeMemoryPool::Instance().Dealloc(ptr);
            });
        });
    }

    deallocer.Fini(true);
    allocer.Fini(true);

    auto ptr = ConcurrentFixedSizeMemoryPool::Instance().Alloc(size);
    ConcurrentFixedSizeMemoryPool::Instance().Dealloc(ptr);
    std::cout << "* _testBasic() done" << std::endl;
}

static void _testMultiThread() {
    std::cout << "* _testMultiThread() started" << std::endl;

    ConcurrentFixedSizeMemoryPool::Instance().Cleanup();
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetSize() == 0);
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() == 0);

    //init
    const uint32_t gcdCount = 100;

    std::list<std::pair<std::shared_ptr<grid::Gcd>, std::list<uint8_t*>>> gcds;
    for (uint32_t i = 0; i < gcdCount; ++i) {
        auto gcd = std::make_shared<grid::Gcd>();
        if (gcd->Init() != 0) {
            std::cerr << "Gcd creation failed" << std::endl;
            return;
        }
        gcds.push_back(std::make_pair(gcd, std::list<uint8_t*>{}));
    }

    //
    const std::vector<uint32_t> memSizes{10000, 20000, 300000, 40000, 50000, 60000, 70000, 80000, 90000, 100000};
    const uint32_t allocCount = memSizes.size() * 1000;

    tbb::atomic<int> doneCount = 0;
    std::mutex mtx;

    for (auto& elem : gcds) {
        auto gcd = elem.first;
        auto& list = elem.second;

        gcd->DispatchAsync([memSizes, allocCount, &doneCount, &list, &mtx]() {
            int idx = 0;
            for (uint32_t i = 0; i < allocCount; ++i) {
                auto n = memSizes[idx++ % memSizes.size()];
                auto ptr = static_cast<uint8_t*>(ConcurrentFixedSizeMemoryPool::Instance().Alloc(n));
                assert(ptr);
                list.push_back(ptr);

                assert(ConcurrentFixedSizeMemoryPool::Instance().GetAvailCount(n) == 0);
            }

            {
                std::lock_guard<std::mutex> guard(mtx);
                std::cout << "doneCount = " << ++doneCount << std::endl;
            }
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

    /*
    uint32_t totalSize = std::accumulate(memSizes.begin(), memSizes.end(), 0) * (allocCount / memSizes.size()) * gcdCount;
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetSize() == totalSize);
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() == memSizes.size());

    for (auto size : memSizes) {
        auto count = (allocCount / memSizes.size()) * gcdCount;
        assert(ConcurrentFixedSizeMemoryPool::Instance().GetAvailCount(size) == count);
    }
     */

    std::cout << "* _testMultiThread() done" << std::endl;
}

static void _testFixedSizeBuffer() {
    std::cout << "* _testFixedSizeBuffer() started" << std::endl;

    ConcurrentFixedSizeMemoryPool::Instance().Cleanup();
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetSize() == 0);
    assert(ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() == 0);

    const uint32_t size = 100;

    grid::Gcd allocer, deallocer;
    assert(allocer.Init() == 0);
    assert(deallocer.Init() == 0);

    for (int i = 0; i < 1000000; ++i) {
        allocer.DispatchAsync([&deallocer]() {
            auto buffer = BufferFactory::MakeFixedSizePoolBuffer(size);
            deallocer.DispatchAsync([buffer]() {
                assert(buffer.Capacity());
            });
        });
    }

    deallocer.Fini(true);
    allocer.Fini(true);

    auto buffer = BufferFactory::MakeFixedSizePoolBuffer(size);
    buffer.Capacity();

    std::cout << "* _testFixedSizeBuffer() done" << std::endl;
}

void grid::TestCFSMemoryPool::DoTest() {
    std::cout << "============= start CFSMemroyPool test ===============" << std::endl;

    //_testBasic();
    _testMultiThread();
    //_testFixedSizeBuffer();

    /*
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "waiting for " << 10 - i << " seconds" << std::endl;
    }
     */

    std::this_thread::sleep_for(std::chrono::seconds(1));


    std::cout << "============= complete CFSMemroyPool test ===============" << std::endl;
}
