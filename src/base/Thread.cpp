//
// Created by lacrimas on 18. 6. 4.
//

#include <base/Thread.h>
#include <base/LogMgr.h>
#include <base/IThreadPoolImpl.h>

using namespace std;
using namespace grid;

Thread::Thread(std::shared_ptr<grid::IThreadPoolImpl> threadPool) : _threadPool(threadPool) {

}

Thread::~Thread() {
    Fini(false);
}

int Thread::Init(const DurationType& precision /* = std::chrono::milliseconds(10) */) {

    _gcd = make_unique<Gcd>();

    auto ret = _gcd->Init(precision);

    if(ret != 0)
    {
        ERROR_LOG("[thread] failed thread init.(err:%d)", ret);
        return ret;
    }

    return 0;
}

int Thread::Fini(bool bWaitUtilAllDispatched /* = false */) {

    if(_gcd)
    {
        auto ret = _gcd->Fini(bWaitUtilAllDispatched);
        _gcd = nullptr;

        return ret;
    }

    return 0;
}

void Thread::Free() {

    if(auto threadPool = _threadPool.lock()) {

        threadPool->Free(shared_from_this());
    }
}

size_t Thread::GetNumOfPendings() const {

    return _gcd ? _gcd->GetNumOfPendings() : 0;
}

void Thread::IncreaseRef() {

    ++_refCnt;
}

void Thread::ReleaseRef() {

    --_refCnt;
}

int Thread::GetRef() {

    int cnt = _refCnt;

    return cnt;
}

int Thread::DestroyTimer(uint32_t id) {

    if(_gcd) {
        return _gcd->DestroyTimer(id);
    }

    return 0;
}