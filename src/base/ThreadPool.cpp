//
// Created by lacrimas on 18. 5. 30.
//

#include <base/ThreadPool.h>
#include <base/ThreadPoolImpl.h>

#include <base/Gcd.h>
#include <base/LogMgr.h>
#include <stdexcept>
#include <base/Thread.h>

using namespace std;
using namespace grid;

//////////////////////////////////////////////////////////////////////
ThreadPool::ThreadPool(ThreadPoolType type) {

    if(type == ThreadPoolType::ROUNDROBIN)
    {
        _pImpl = make_unique<StlRoundRobinImpl>();
    }
    else if(type == ThreadPoolType::REFBASED)
    {
        _pImpl = make_unique<StlRefBasedImpl>();
    }
    else
    {
        throw std::runtime_error("invalid threadpool type");
    }
}

ThreadPool::~ThreadPool() {

    Uninit();
}

int ThreadPool::Init(int cnt) {

    return _pImpl->Init(cnt);
}

void ThreadPool::Uninit() {

    if(_pImpl) {

        _pImpl->Uninit();
        _pImpl = nullptr;
    }
}

ThreadPtr ThreadPool::Alloc() {

    return _pImpl->Alloc();
}

void ThreadPool::Free(ThreadPtr thr) {

    _pImpl->Free(thr);
}
