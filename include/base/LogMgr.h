#pragma once

#include <base/Gcd.h>
#include <atomic>
#include <list>
#include <set>
#include <string>
#include <iostream>

#include <mutex>
#include <shared_mutex>

#include <base/Base.h>
#include <base/StringHelper.h>

#include <assert.h>

#define DEBUG_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_DEBUG, __VA_ARGS__)
#define INFO_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_INFO, __VA_ARGS__)
#define WARN_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_WARN, __VA_ARGS__)
#define ERROR_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_ERROR, __VA_ARGS__)
#define FATAL_LOG(...)							grid::log::LogMgr::Instance().WriteLog(__TFILE__, __LINE__, (int)grid::log::LOG_FATAL, __VA_ARGS__)

#define INIT_LOG_MGR(level,type)				grid::log::LogMgr::Instance().Init(level, type)
#define UNINIT_LOG_MGR()						grid::log::LogMgr::Instance().Uninit()

#define REGISTER_LOG_WRITER(writer)				grid::log::LogMgr::Instance().RegisterLogWriter(writer)
#define UNREGISTER_LOG_WRITER(writer)			grid::log::LogMgr::Instance().UnregisterLogWriter(writer)

namespace grid {
	namespace log {

		enum LOG_LEVEL {
			LOG_DEBUG = 1,
			LOG_INFO = 2,
			LOG_WARN = 3,
			LOG_ERROR = 4,
			LOG_FATAL = 5,
		};

		enum LOG_WRITER_TYPE {
			LOG_WRITER_CONSOLE			= 0x0001,
			LOG_WRITER_OUTPUT_DEBUG		= 0x0010,
			LOG_WRITER_FILE				= 0x0100,

			LOG_WRITER_ALL = LOG_WRITER_CONSOLE | LOG_WRITER_OUTPUT_DEBUG | LOG_WRITER_FILE
		};

		class ILogWriter;
		///////////////////////////////////////////////////////////////////////
		class LogMgr  {

			using ILogWriterPtr = std::shared_ptr<ILogWriter>;
			using LogWriterList = std::set<ILogWriterPtr>;
			using LogWriterListItr = LogWriterList::iterator;
			using LogLevel = std::atomic_char;
			using MUTEX_TYPE = std::mutex;

		protected:
			LogMgr();
		public:
			static LogMgr& Instance() {
				static LogMgr logMgr;

				return logMgr;
			}

			~LogMgr();

			bool Init(int level, int type = LOG_WRITER_ALL);
			void Uninit();

			void SetLevel(uint16_t level);

			void RegisterLogWriter(const ILogWriterPtr& pLogWriter);
			void UnregisterLogWriter(const ILogWriterPtr& pLogWriter);

			template<class ...Args>
			void WriteLog(const tchar * szFileName, int nLine, int level, const tchar * pszFmt, Args&&... args) {

				if (m_LogLevel.load() > level) {

					return;
				}

				tstring strTid = _GetCurrentThreadId();
				tstring strTime = _GetCurrentTimeStr();
				time_t curTime = _GetTime();

				tstring strBuffer;
				util::StringHelper::Format(strBuffer, pszFmt, args ...);

				// read lock
				std::lock_guard<MUTEX_TYPE> lock(m_gcdMutex);
				if (m_pGcd != nullptr) {

					m_pGcd->DispatchAsync(&LogMgr::_Evt_Write, this, tstring(szFileName), nLine, level, curTime, strTid, strTime, strBuffer);
				}
			}

		private:
			bool _IsSet(int type, int mask);
			void _RegisterLogger(int type);
			int _UnregisterLogger();

			time_t _GetTime();
			tstring _GetCurrentTimeStr();
			tstring _GetCurrentThreadId();

			tstring _GetLogLevelStr(int level) const;

			void _Evt_Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg);

			std::shared_ptr<Gcd> m_pGcd;
			std::atomic_int		 m_LogLevel = {0};

			MUTEX_TYPE			 m_gcdMutex;
			MUTEX_TYPE			 m_lstWriterMutext;

			LogWriterList		 m_lstWriter;


			static const int	 MAX_LOG_BUFFER = 8192;
		};
	} // namespace log
} // namespace grid

