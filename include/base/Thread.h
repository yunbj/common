//
// Created by lacrimas on 18. 6. 4.
//

#ifndef GRID_COMMON_THREAD_H
#define GRID_COMMON_THREAD_H

#include <memory>
#include <chrono>
#include <functional>
#include <atomic>

#include <base/Gcd.h>

namespace grid
{
    class StlRoundRobinImpl;
    class StlRefBasedImpl;
    class Thread {
        friend StlRoundRobinImpl;
        friend StlRefBasedImpl;

        using DurationType = std::chrono::milliseconds;
        using FnType = std::function<void(void)>;

        using Gcd = grid::Gcd;
        using GcdPtr = std::unique_ptr<Gcd>;

    public:
        Thread() {};
        ~Thread();

        int Init(const DurationType &precision = std::chrono::milliseconds(10));

        size_t GetNumOfPendings() const;

        void IncreaseRef();
        void ReleaseRef();

        int GetRef();

        template<class Fn, class...Args>
        int DispatchAsync(Fn &&fn, Args &&... args) {
            return _gcd->DispatchAsync(std::forward<Fn>(fn), std::forward<Args>(args)...);
        }

        template<class Fn, class...Args>
        decltype(auto) DispatchSync(Fn &&fn, Args &&... args) {
            return _gcd->DispatchSync(std::forward<Fn>(fn), std::forward<Args>(args)...);
        };

        template<class Fn, class...Args>
        uint32_t CreateTimer(const DurationType& interval, bool repeat, Fn&& fn, Args&&... args) {
            if(_gcd == nullptr)
            {
                // TODO: return error code
                return -1;
            }
            return _gcd->CreateTimer(interval, interval, repeat, std::forward<Fn>(fn), std::forward<Args>(args)...);
        }

        template<class Fn, class...Args>
        uint32_t CreateTimer(const DurationType& after, const DurationType& interval, bool repeat, Fn&& fn, Args&&... args) {
            if(_gcd == nullptr)
            {
                // TODO: return error code
                return -1;
            }

            return _gcd->_CreateTimer(after, interval, repeat, std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        }

        int DestroyTimer(uint32_t id);

    private:
        // for threadpool
        int Fini(bool bWaitUtilAllDispatched = false);

        GcdPtr _gcd;
        std::atomic_int _refCnt = 0;

    };

    using ThreadPtr = std::shared_ptr<Thread>;
} // namespace grid

#endif //GRID_COMMON_THREAD_H
