//
// Created by lacrimas on 18. 5. 31.
//

#include <base/ThreadPoolImpl.h>
#include <base/LogMgr.h>
#include <base/Thread.h>

#include <vector>
#include <atomic>
#include <memory>
#include <map>

using namespace std;
using namespace grid;

////////////////////////////////////////////////////////////////////////////////////////////////////
// StlRoundRobinImpl Implementaion

struct StlRoundRobinImpl::Impl {

    int32_t                 thrCnt = 0;
    std::atomic_int         curAlloc = 0;      // 현재 allocate된 thread 개수

    std::atomic_int         index = 0;
    std::vector<ThreadPtr>  threads;
};

/////////////////////////////////////////////////////////////////
StlRoundRobinImpl::StlRoundRobinImpl()
: _pImpl(make_unique<StlRoundRobinImpl::Impl>()) {
}

StlRoundRobinImpl::~StlRoundRobinImpl() {
}

int StlRoundRobinImpl::Init(int32_t cnt) {
    _pImpl->thrCnt = cnt;
    _pImpl->threads.resize(cnt);

    for(auto i = 0; i < cnt; ++i)
    {
        ThreadPtr thr = make_shared<Thread>(shared_from_this());
        _pImpl->threads[i] = thr;

        auto ret = thr->Init();

        if(ret != 0)
        {
            ERROR_LOG("[threadpool] failed init gcd.(err:%d)", ret);
            _pImpl->threads.clear();

            return -1;
        }
    }

    INFO_LOG("[threadpool] complete thread pool init.(pool:%d)", cnt);

    return 0;
}

void StlRoundRobinImpl::Uninit()
{
    if(_pImpl == nullptr) {

        return;
    }

    for(size_t i = 0; i < _pImpl->threads.size(); ++i) {

        _pImpl->threads[i]->Fini(false);
    }

    _pImpl->threads.clear();

    INFO_LOG("[threadpool] complete uninit thread pool.(pool:%d)", _pImpl->thrCnt);
}

ThreadPtr StlRoundRobinImpl::Alloc() {

    auto cnt = ++_pImpl->curAlloc;
    DEBUG_LOG("[threadpool] allocate thread. (cur:%d, pool:%d)", cnt, _pImpl->thrCnt);

    auto index = ((_pImpl->index++)  % _pImpl->thrCnt );
    return _pImpl->threads[index];
}

void StlRoundRobinImpl::Free(ThreadPtr thr) {

    auto cnt = --_pImpl->curAlloc;
    DEBUG_LOG("[threadpool] free thread. (allocated thread:%d, pool:%d)", cnt, _pImpl->thrCnt);
}

int32_t StlRoundRobinImpl::GetAllocatedThreadCount()
{
    return _pImpl->curAlloc;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// StlRefBasedImpl Implementaion

struct StlRefBasedImpl::Impl {

    int32_t         thrCnt = 0;
    std::atomic_int curAlloc = 0;      // 현재 allocate된 thread 개수

    mutex           mtx;
    map<int, set<ThreadPtr> > threads;
};

using LockGuard = lock_guard<mutex>;
StlRefBasedImpl::StlRefBasedImpl()
        : _pImpl(make_unique<StlRefBasedImpl::Impl>()) {
}

StlRefBasedImpl::~StlRefBasedImpl() {

}

int StlRefBasedImpl::Init(int32_t cnt) {

    LockGuard guard(_pImpl->mtx);

    _pImpl->thrCnt = cnt;

    for (auto i = 0; i < cnt; ++i)
    {
        ThreadPtr thr = make_shared<Thread>(shared_from_this());

        auto ret = thr->Init();
        if (ret != 0)
        {
            ERROR_LOG("[threadpool] failed init gcd.(err:%d)", ret);
            _pImpl->threads.clear();

            return -1;
        }

        _pImpl->threads[0].insert(thr);
    }

    INFO_LOG("[threadpool] complete thread pool init.(pool:%d)", cnt);

    return 0;
}

void StlRefBasedImpl::Uninit() {

    if(_pImpl == nullptr)
    {
        return;
    }

    map<int32_t, set<ThreadPtr> > threads;
    {
        LockGuard guard(_pImpl->mtx);
        threads = move(_pImpl->threads);
    }

    for (auto& it : threads)
    {
        for (auto thr : it.second)
        {
            thr->Fini(false);
        }
    }

    INFO_LOG("[threadpool] complete uninit thread pool.(pool:%d)", _pImpl->thrCnt);
}

ThreadPtr StlRefBasedImpl::Alloc() {

    LockGuard guard(_pImpl->mtx);
    auto& threads = _pImpl->threads;

    for (auto& it : threads)
    {
        auto refCnt = it.first;
        auto& lst = it.second;

        for (auto thr : lst)
        {
            // ref + 1 한 위치에 추가
            threads[refCnt + 1].insert(thr);

            // 기존 ref 위치에서존 제거
            lst.erase(thr);
            if (lst.empty())
            {
                threads.erase(refCnt);
            }
            thr->IncreaseRef();

            // debug
            auto cnt = ++_pImpl->curAlloc;
            DEBUG_LOG("[threadpool] allocate thread. (cur:%d, pool:%d)", cnt, _pImpl->thrCnt);

            return thr;
        }
    }

    ERROR_LOG("[threadpool] failed allocate thread.");
    return nullptr;
}

void StlRefBasedImpl::Free(ThreadPtr thr) {

    LockGuard guard(_pImpl->mtx);
    auto& threads = _pImpl->threads;

    auto refCnt = thr->GetRef();
    auto it = threads.find(refCnt);

    if (refCnt == 0)
    {
        assert(!"invalid thread ref count");
        ERROR_LOG("[threadpool] failed free thread. (thread ref count is zero).");
        return;
    }

    if (it == threads.end())
    {
        assert(!"invalid ref count");
        ERROR_LOG("[threadpool] failed free thread. (not found thread ref count in pool.(ref:%d)).", refCnt);
        return;
    }

    threads[refCnt - 1].insert(thr);

    auto& lst = it->second;
    lst.erase(thr);

    thr->ReleaseRef();

    // debug
    auto cnt = --_pImpl->curAlloc;
    DEBUG_LOG("[threadpool] free thread. (allocated thread:%d, pool:%d)", cnt, _pImpl->thrCnt);
}


int32_t StlRefBasedImpl::GetAllocatedThreadCount()
{
    return _pImpl->curAlloc;
}
