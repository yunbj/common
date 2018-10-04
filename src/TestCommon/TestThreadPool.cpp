//
// Created by lacrimas on 18. 8. 7.
//

#include "TestThreadPool.h"
#include <base/ThreadPool.h>
#include <base/Thread.h>
#include <assert.h>

using namespace std;
using namespace grid;

TestThreadPool::TestThreadPool() {
    //ctor
}

TestThreadPool::~TestThreadPool() {
    //dtor
}

void TestThreadPool::DoTest() {

    std::unique_ptr<ThreadPool> pool = make_unique<ThreadPool>();

    assert(pool->Init(10) == 0);

    auto thr = pool->Alloc();
    pool->Free(thr);

    assert(pool->GetAllocatedThreadCount() == 0);

    std::shared_ptr<Thread> selfFree = pool->Alloc();
    selfFree->Free();

    assert(pool->GetAllocatedThreadCount() == 0);
}