#pragma once

#include <base/Gcd.h>
#include <atomic>
#include <list>
#include <set>
#include <string>
#include <iostream>

#include <mutex>
#include <shared_mutex>
#include <vector>

#include <base/Base.h>
#include <base/StringHelper.h>
#include <base/ILogWriter.h>
#include <base/LogThread.h>

#include <assert.h>

#define DEBUG_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_DEBUG, __VA_ARGS__)
#define INFO_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_INFO, __VA_ARGS__)
#define WARN_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_WARN, __VA_ARGS__)
#define ERROR_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_ERROR, __VA_ARGS__)
#define FATAL_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_FATAL, __VA_ARGS__)

#define INIT_LOG_MGR(level,type, thrCnt)		grid::log::LogMgr::Instance().Init(level, type, thrCnt)
#define UNINIT_LOG_MGR()						grid::log::LogMgr::Instance().Uninit()
#define RESIZE_LOG_THREAD(cnt)					grid::log::LogMgr::Instance().Resize(cnt)

#define REGISTER_LOG_WRITER(writer)				grid::log::LogMgr::Instance().RegisterLogWriter(writer)
#define UNREGISTER_LOG_WRITER(writer)			grid::log::LogMgr::Instance().UnregisterLogWriter(writer)

namespace grid {
	namespace log {

		///////////////////////////////////////////////////////////////////////
		class LogMgr  {

			using ILogWriterPtr = std::shared_ptr<ILogWriter>;
			using LogWriterList = std::set<ILogWriterPtr>;
			using LogWriterListItr = LogWriterList::iterator;

			using LogLevel = std::atomic_char;
			using MUTEX_TYPE = std::mutex;

			using RW_MUTEX_TYPE = std::shared_mutex;
			using READ_LOCKGUARD_TYPE = std::shared_lock<std::shared_mutex>;
			using WRITE_LOCKGUARD_TYPE = std::unique_lock<std::shared_mutex>;

			using LogThreadPtr = std::shared_ptr<LogThread>;
			using LogThreads = std::vector<LogThreadPtr>;

		protected:
			LogMgr();
		public:
			static LogMgr& Instance() {
				static LogMgr logMgr;

				return logMgr;
			}

			~LogMgr();

			bool Init(int level, int type = LOG_WRITER_ALL, size_t thrCount = 1);
			void Uninit();

			void SetLevel(uint16_t level);
			void Resize(size_t thrCount);

			void RegisterLogWriter(const ILogWriterPtr& writer);
			void UnregisterLogWriter(const ILogWriterPtr& writer);

			template<class ...Args>
			void WriteLog(const tchar * fileName, int line, int level, const tchar * pszFmt, Args&&... args) {

				if (_logLevel.load() > level || _destroyed == true) {

					return;
				}

				tstring tid = _GetCurrentThreadId();
				tstring st;
				std::time_t curTime = _GetCurrentTimeStr(st);

				LogThreadPtr thr;
				{
					READ_LOCKGUARD_TYPE lock(_mtxThreads);
					if(_logThreads.empty()) {
						return;
					}

					int index = (_curThreadIndex) % _logThreads.size();
					thr = _logThreads[index];

					++_curThreadIndex;
				}

				tstring buffer;
				util::StringHelper::Format(buffer, pszFmt, args ...);

				thr->Write(++_logSeq, fileName, line, level, curTime, tid, st, buffer);
			}

		private:

			std::time_t _GetCurrentTimeStr(std::string& curTime);
			tstring _GetCurrentThreadId();

			std::atomic_bool	_destroyed = {false};
			std::atomic_uint16_t _logSeq = {0};
			std::atomic_int		_logLevel = {0};

			int 				_logType = {0};
			int     			_curThreadIndex = {0};

			RW_MUTEX_TYPE		 _mtxThreads;
			MUTEX_TYPE			 _mtxConsole;

			LogThreads			_logThreads;
		};
	} // namespace log
} // namespace grid

