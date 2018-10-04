//
// Created by lacrimas on 18. 5. 30.
//

#ifndef FILEMANAGER_THREADPOOL_H
#define FILEMANAGER_THREADPOOL_H

#include <memory>
#include <base/IThreadPoolImpl.h>

namespace grid
{
    enum class ThreadPoolType {
        ROUNDROBIN,
        REFBASED
    };

    class Gcd;
    class Thread;
    //////////////////////////////////////////////////////////////////
    // thread pool interface
    class IThreadPool  {
    protected:
        using ImplPtr = std::shared_ptr<IThreadPoolImpl>;
        using ThreadPtr = std::shared_ptr<Thread>;

    public :
        IThreadPool() = default;
        virtual ~IThreadPool() = default;

        virtual int Init(int32_t cnt) = 0;
        virtual void Uninit() = 0;

        virtual ThreadPtr Alloc() = 0;
        virtual void Free(ThreadPtr thr) = 0;
        virtual int32_t GetAllocatedThreadCount() = 0;
    };

    //////////////////////////////////////////////////////////////////
    // thread pool
    class ThreadPool : public IThreadPool {
    public :

        ThreadPool(ThreadPoolType type = ThreadPoolType::ROUNDROBIN);
        virtual ~ThreadPool();

        virtual int Init(int32_t cnt) override;
        virtual void Uninit() override;

        virtual ThreadPtr Alloc() override;
        virtual void Free(ThreadPtr thr) override;

        int32_t GetAllocatedThreadCount() override ;

    private:
        ImplPtr _pImpl;
    };
} // namespace dva

#endif //FILEMANAGER_THREADPOOL_H
