#pragma once

#include <memory>
#include <chrono>
#include <functional>
#include <mutex>
#include <type_traits>

namespace grid
{
	class Gcd
	{
	public:
		using DurationType = std::chrono::milliseconds;
		using FnType = std::function<void(void)>;
		using MutexType = std::mutex;

	public:
		Gcd();

		~Gcd();

		int Init(const DurationType& precision = std::chrono::milliseconds(10));

		int Fini(bool bWaitUtilAllDispatched = false);

		bool IsRunning() const;

		std::size_t GetNumOfPendings() const;

		template<class Fn, class...Args>
		int DispatchAsync(Fn&& fn, Args&&... args)
		{
			return this->_DispatchAsync(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
		}

		template<class Fn, class...Args>
		decltype(auto) DispatchSync(Fn&& fn, Args&&... args)
		{
			using RetType = std::result_of_t<Fn(Args...)>;

			if (!this->IsRunning())
			{
				return RetType();
			}

			auto binded = std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...);
			RetType ret;

			auto lock = this->_AcquireSyncLock();
            bool comp = false;
            
			this->DispatchAsync([this, &ret, &binded, &comp]()
			{
				try
				{
					ret = binded();
				}
				catch (...)
				{
				}

				this->_NotifySync(comp);
			});

			this->_WaitSync(lock, comp);
			return ret;
		}

		template<class Fn, class...Args>
		uint32_t CreateTimer(const DurationType& interval, bool repeat, Fn&& fn, Args&&... args)
		{
			return this->CreateTimer(interval, interval, repeat, std::forward<Fn>(fn), std::forward<Args>(args)...);
		}

        template<class Fn, class...Args>
        uint32_t CreateTimer(const DurationType& after, const DurationType& interval, bool repeat, Fn&& fn, Args&&... args)
        {
            return this->_CreateTimer(after, interval, repeat, std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
        }
        
		int DestroyTimer(uint32_t id);

	private:
		int _DispatchAsync(const FnType& fn);

		uint32_t _CreateTimer(const DurationType& interval, bool repeat, const FnType& fn);

        uint32_t _CreateTimer(const DurationType& after, const DurationType& interval, bool repeat, const FnType& fn);
        
		std::unique_lock<MutexType> _AcquireSyncLock();

		void _NotifySync(bool& comp);

		void _WaitSync(std::unique_lock<MutexType>& lock, bool& comp);

	private:
		class _Impl;
		std::unique_ptr<_Impl> _pImpl;
	};
}
