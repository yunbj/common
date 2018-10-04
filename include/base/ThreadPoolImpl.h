//
// Created by lacrimas on 18. 5. 31.
//

#ifndef FILEMANAGER_THREADPOOLIMPL_H
#define FILEMANAGER_THREADPOOLIMPL_H

#include "IThreadPoolImpl.h"

namespace grid
{
    class StlRoundRobinImpl : public IThreadPoolImpl, public std::enable_shared_from_this<StlRoundRobinImpl>
    {
    public:
        StlRoundRobinImpl();
        virtual ~StlRoundRobinImpl();

        virtual int Init(int32_t cnt) override;
        virtual void Uninit() override;

        virtual ThreadPtr Alloc() override;
        virtual void Free(ThreadPtr thr) override;

        virtual int32_t GetAllocatedThreadCount() override;

    private:
        struct Impl;
        std::unique_ptr<Impl>   _pImpl;
    };


    class StlRefBasedImpl : public IThreadPoolImpl, public std::enable_shared_from_this<StlRefBasedImpl>
    {
    public:
        StlRefBasedImpl();
        virtual ~StlRefBasedImpl();

        virtual int Init(int32_t cnt) override;
        virtual void Uninit() override;

        virtual ThreadPtr Alloc() override;
        virtual void Free(ThreadPtr thr) override;

        virtual int32_t GetAllocatedThreadCount() override;

    private:
        struct Impl;
        std::unique_ptr<Impl>   _pImpl;
    };
} // namespace dva

#endif //FILEMANAGER_THREADPOOLIMPL_H
