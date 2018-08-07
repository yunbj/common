//
// Created by lacrimas on 18. 5. 31.
//

#ifndef GRID_COMMON_ITHREADPOOLIMPL_H
#define GRID_COMMON_ITHREADPOOLIMPL_H

#include <memory>

namespace grid {

    class Thread;
    class IThreadPoolImpl {
    protected:
        using ThreadPtr = std::shared_ptr<Thread>;
    public:
        IThreadPoolImpl() = default;
        virtual ~IThreadPoolImpl() = default;

        virtual int Init(int cnt) = 0;
        virtual void Uninit() = 0;

        virtual ThreadPtr Alloc() = 0;
        virtual void Free(ThreadPtr thr) = 0;
    };
} // namespace grid


#endif //GRID_COMMON_ITHREADPOOLIMPL_H
