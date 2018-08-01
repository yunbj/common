#include "base/Gcd.h"
#include <list>
#include <map>
#include <future>
#include <atomic>
#include <condition_variable>
#include <cassert>
#include <ctime>

namespace grid
{
	
	class Gcd::_Impl
	{
	public://alias
		using ClockType = std::chrono::high_resolution_clock;
		using DurationType = Gcd::DurationType;
		using FnType = Gcd::FnType;
		using MutexType = Gcd::MutexType;

	public://functions
		int Init(const DurationType& precision);

		int Fini();

		std::size_t GetNumOfPendings() const;

		int DispatchAsync(FnType&& fn);

		uint32_t CreateTimer(const DurationType& interval, bool repeat, FnType&& fn);

		uint32_t CreateTimer(const DurationType& after, const DurationType& interval, bool repeat, FnType&& fn);
        
		int DestroyTimer(uint32_t id);

		std::unique_lock<MutexType> _AcquireSyncLock();

		void _NotifySync(bool& comp);

		void _WaitSync(std::unique_lock<MutexType>& lock, bool& comp);

	private:
		void _ProcessTimer(std::unique_lock<MutexType>& lock);

		void _ProcessCommand(std::unique_lock<MutexType>& lock);

		int _ThreadFunc();

	public:
		struct _TimerValue
		{
			uint32_t _id;
			bool _repeat;
			Gcd::DurationType _interval;
			Gcd::FnType _fn;

			_TimerValue()
				: _id(0)
				, _repeat(false)
				, _interval()
				, _fn()
			{}

			_TimerValue(uint32_t id, bool repeat, const Gcd::DurationType& interval, Gcd::FnType&& fn)
				: _id(id)
				, _repeat(repeat)
				, _interval(interval)
				, _fn(std::move(fn))
			{}
		};

	public:
		DurationType _precision;
		std::list<FnType> _q;

		mutable MutexType _mtxForThread;
		mutable MutexType _mtxForSync;
		std::atomic_bool _bRunning = {false};
		std::future<int> _fut;
		std::condition_variable _cvEvent;
		std::condition_variable _cvSync;

		std::multimap<ClockType::time_point, struct _TimerValue> _timers;
		uint32_t _nNextTimerId = 1;
	};


	int Gcd::_Impl::Init(const DurationType& precision)
	{
		if (_bRunning)
		{
			return -1;
		}

		_precision = precision;
		_bRunning = true;
        _fut = std::async(std::launch::async, &_Impl::_ThreadFunc, this);

		if (_fut.wait_for(std::chrono::seconds(0)) == std::future_status::deferred)
		{
			_bRunning = false;
			return -1;
		}

		return 0;
	}

	int Gcd::_Impl::Fini()
	{
		//wake up _cvEvent
		{
			std::lock_guard<std::mutex> lock(_mtxForThread);
			_bRunning = false;
		}
		_cvEvent.notify_one();

		//wake up _cvSync
		//this->_NotifySync();

		_fut.wait();
		return 0;
	}

	std::size_t Gcd::_Impl::GetNumOfPendings() const
	{
		std::lock_guard<std::mutex> lock(_mtxForThread);
		return _q.size();
	}

	int Gcd::_Impl::DispatchAsync(FnType&& fn)
	{
		if (!_bRunning)
		{
			return -1;
		}

		{
			std::unique_lock<MutexType> lock(_mtxForThread);
			_q.emplace_back(std::move(fn));
		}

		_cvEvent.notify_one();
		return 0;
	}

	
	uint32_t Gcd::_Impl::CreateTimer(const DurationType& interval, bool repeat, FnType&& fn)
	{
		/*
		if (!_bRunning)
		{
			return 0;
		}
		*/

		auto expiredAt = ClockType::now() + interval;

		uint32_t id = 0;

		{
			std::unique_lock<MutexType> lock(_mtxForThread);

			id = _nNextTimerId++;
			_timers.insert(std::make_pair(expiredAt, _TimerValue(id, repeat, interval, std::move(fn))));
		}

		_cvEvent.notify_one();
		return id;
	}

    uint32_t Gcd::_Impl::CreateTimer(const DurationType& after, const DurationType& interval, bool repeat, FnType&& fn)
    {
        auto expiredAt = ClockType::now() + after;
        
        uint32_t id = 0;
        
        {
            std::unique_lock<MutexType> lock(_mtxForThread);
            
            id = _nNextTimerId++;
			_timers.insert(std::make_pair(expiredAt, _TimerValue(id, repeat, interval, std::move(fn))));
        }
        
        _cvEvent.notify_one();
        return id;
    }
    
	int Gcd::_Impl::DestroyTimer(uint32_t id)
	{
		int nRet = -1;

		{
			std::unique_lock<MutexType> lock(_mtxForThread);

			for (auto it = _timers.begin(); it != _timers.end(); ++it)
			{
				if (it->second._id == id)
				{
					_timers.erase(it);
					nRet = 0;
					break;
				}
			}
		}

		_cvEvent.notify_one();
		return nRet;
	}

	std::unique_lock<Gcd::MutexType> Gcd::_Impl::_AcquireSyncLock()
	{
		std::unique_lock<MutexType> lock(_mtxForSync);
		return lock;
	}

	void Gcd::_Impl::_NotifySync(bool& comp)
	{
		//lock already released

		{
			std::lock_guard<std::mutex> lock(_mtxForSync);
			comp = true;
		}

		_cvSync.notify_one();
	}

	void Gcd::_Impl::_WaitSync(std::unique_lock<MutexType>& lock, bool& comp)
	{
		//lock already acquired
        
        auto pred = [this, &comp]() { return !_bRunning || comp; };
        
		while (!pred())
		{
            _cvSync.wait_for(lock, std::chrono::milliseconds(100), [&pred]() { return pred(); });
		}
	}

	int Gcd::_Impl::_ThreadFunc()
	{
		std::srand(static_cast<uint32_t>(std::time(nullptr)));

		std::unique_lock<MutexType> lock(_mtxForThread);

		while (_bRunning)
		{
			this->_ProcessCommand(lock);

			this->_ProcessTimer(lock);

            _cvEvent.wait_for(lock, _precision, [this ]{ return !_q.empty(); });
		}

		_q.clear();
		_timers.clear();

		return 0;
	};

	void Gcd::_Impl::_ProcessTimer(std::unique_lock<MutexType>& lock)
	{
		//already lock acquired
		if (_timers.empty())
		{
			return;
		}

		const auto now = ClockType::now();
		auto endIt = _timers.lower_bound(now);

		if (endIt != _timers.begin())
		{
			//lock scope
			std::list<_TimerValue> expiredTimers;
			for (auto it = _timers.begin(); it != endIt; )
			{
				expiredTimers.emplace_back(std::move(it->second));
				_timers.erase(it++);
			}

			//unlock scope
			lock.unlock();
			std::list<_TimerValue> repeatTimers;
			for (auto& tv : expiredTimers)
			{
				if (!_bRunning)
				{
					break;
				}

				try
				{
					tv._fn();
				}
				catch (...)
				{ }

				if (tv._repeat)
				{
					repeatTimers.emplace_back(std::move(tv));
				}
			}

			//lock scope
			lock.lock();
			for (auto& tv : repeatTimers)
			{
				_timers.insert(std::make_pair(now + tv._interval, std::move(tv)));
			}
		}
	}

	void Gcd::_Impl::_ProcessCommand(std::unique_lock<MutexType>& lock)
	{
		//already lock acquired
		if (_bRunning && !_q.empty())
		{
			auto copiedQueue = std::move(_q);
			lock.unlock();

			for (auto it = copiedQueue.begin(); _bRunning && it != copiedQueue.end(); ++it)
			{
				try
				{
					(*it)();
				}
				catch (...)
				{
				}
			}

			lock.lock();
		}
	}



	/*
	 * Gcd class
	 */
	Gcd::Gcd()
		: _pImpl(std::make_unique<_Impl>())
	{}

	Gcd::~Gcd()
	{
		this->Fini();
	}

	int Gcd::Init(const DurationType& precision)
	{
		return _pImpl->Init(precision);
	}

	int Gcd::Fini(bool bWaitUtilAllDispatched)
	{
		if (!this->IsRunning())
		{
			return 0;
		}

		if (bWaitUtilAllDispatched)
		{
			this->DispatchSync([]() { return 0; });
		}

		return _pImpl->Fini();
	}

	bool Gcd::IsRunning() const
	{
		return _pImpl->_bRunning;
	}

	std::size_t Gcd::GetNumOfPendings() const
	{
		return _pImpl->GetNumOfPendings();
	}

	int Gcd::DestroyTimer(uint32_t id)
	{
		return _pImpl->DestroyTimer(id);
	}

	int Gcd::_DispatchAsync(FnType&& fn)
	{
		return _pImpl->DispatchAsync(std::forward<FnType>(fn));
	}

	uint32_t Gcd::_CreateTimer(const DurationType& interval, bool repeat, FnType&& fn)
	{
		return _pImpl->CreateTimer(interval, repeat, std::forward<FnType>(fn));
	}

	uint32_t Gcd::_CreateTimer(const DurationType& after, const DurationType& interval, bool repeat, FnType&& fn)
	{
		return _pImpl->CreateTimer(after, interval, repeat, std::forward<FnType>(fn));
    }
    
	std::unique_lock<Gcd::MutexType> Gcd::_AcquireSyncLock()
	{
		return _pImpl->_AcquireSyncLock();
	}

	void Gcd::_NotifySync(bool& comp)
	{
		_pImpl->_NotifySync(comp);
	}

	void Gcd::_WaitSync(std::unique_lock<MutexType>& lock, bool& comp)
	{
		_pImpl->_WaitSync(lock, comp);
	}
}
