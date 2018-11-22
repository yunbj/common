#include <base/LogMgr.h>
#include <thread>
#include <string>
#include <chrono>
#include <sstream>
#include <time.h>	// localtime
#include <iomanip>  // put_time
#include <stdarg.h>
#include <iostream>
#include <algorithm>

#include <base/Base.h>
#include <base/ILogWriter.h>
#include <base/FileSystem.h>
#include <base/StringHelper.h>

#include <assert.h>

#if defined (_WIN32) || defined(_WIN64)
#include <Windows.h>
#define thread_safe_localtime(time, time_info)		localtime_s(time_info, time)
#define DIR_DELIMETER   _T("\\")
#else // WIN32
#define thread_safe_localtime(time, time_info)		localtime_r(time, time_info)
#define DIR_DELIMETER   _T("/")
#endif // WIN32

using namespace grid;
using namespace grid::log;

///////////////////////////////////////////////////////////////////////////////////////////
LogMgr::LogMgr()
	: _logLevel(LOG_DEBUG) {

}


LogMgr::~LogMgr() {
	Uninit();
}

bool LogMgr::Init(int level, int type, size_t thrCnt) {

	_logLevel = level;
	_logType = type;

	// write lock
	WRITE_LOCKGUARD_TYPE lock(_mtxThreads);

	if(!_logThreads.empty()){
		return false;
	}

	_logThreads.resize(thrCnt);
	for(size_t i = 0; i < thrCnt; ++i) {

		_logThreads[i] = std::make_shared<LogThread>(_mtxConsole, i);
		_logThreads[i]->Init(type);
	}

	std::string localName = util::FileSystem::GetSystemLocaleName();
	tcout.imbue(std::locale(localName.c_str()));

	return true;
}

void LogMgr::Uninit() {

	if(_destroyed) {
		return;
	}

	_destroyed = true;

	// write lock
	WRITE_LOCKGUARD_TYPE lock(_mtxThreads);
	auto logThreads = std::move(_logThreads);

	for(size_t i = 0; i < logThreads.size(); ++i) {

		logThreads[i]->Uninit();
	}

	logThreads.clear();

	tcout.imbue(std::locale("C"));
}

void LogMgr::SetLevel(uint16_t level) {

	_logLevel.store(level);
}

void LogMgr::Resize(size_t newCnt) {

	size_t curCnt = 0;
	{
		WRITE_LOCKGUARD_TYPE lock(_mtxThreads);
		curCnt = _logThreads.size();

		if(newCnt == 0 || curCnt == newCnt) {
			return;
		}

		if(newCnt < curCnt) {

			LogThreads threads;
			threads.insert(threads.begin(), _logThreads.begin(), _logThreads.begin() + newCnt);

			for(auto i = newCnt; i < curCnt; ++i) {

				_logThreads[i]->Uninit();
			}

			_logThreads = std::move(threads);

		}
		else if(newCnt > curCnt) {

			_logThreads.resize(newCnt);
			for(auto i = curCnt; i < newCnt; ++i) {

				_logThreads[i] = std::make_shared<LogThread>(_mtxConsole, i);
				_logThreads[i]->Init(_logType);
			}
		}
	}
}

void LogMgr::RegisterLogWriter(const ILogWriterPtr& writer) {

	// write lock
	WRITE_LOCKGUARD_TYPE lock(_mtxThreads);
	for(size_t i = 0; i < _logThreads.size(); ++i) {

		_logThreads[i]->RegisterLogWriter(writer);
	}

}

void LogMgr::UnregisterLogWriter(const ILogWriterPtr& writer) {

	// write lock
	WRITE_LOCKGUARD_TYPE lock(_mtxThreads);
	for(size_t i = 0; i < _logThreads.size(); ++i) {

		_logThreads[i]->UnregisterLogWriter(writer);
	}

}

std::time_t LogMgr::_GetCurrentTimeStr(std::string& curTime) {

	using namespace std::chrono;

	auto p = system_clock::now();
	auto ms = duration_cast<milliseconds>(p.time_since_epoch());
	auto s = duration_cast<seconds>(ms);
	std::time_t t = s.count();

	struct tm st;
	thread_safe_localtime(&t, &st);

	FORMAT(curTime, _T("%04d-%02d-%02d_%02d:%02d:%02d.%03d"),
		   st.tm_year + 1900,
		   st.tm_mon + 1,
		   st.tm_mday,
		   st.tm_hour,
		   st.tm_min,
		   st.tm_sec,
		   (unsigned)(ms.count() % 1000));

	return t;
}

tstring LogMgr::_GetCurrentThreadId() {

	tstringstream ss;

	ss << std::this_thread::get_id();

	return ss.str();
}