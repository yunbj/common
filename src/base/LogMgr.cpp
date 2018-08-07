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


#if defined(WIN32) || defined(_WIN64)
class ConsoleTextColor
{
public:
	enum {
		black,
		dark_blue,
		dark_green,
		dark_cyan,
		dark_red,
		dark_magenta,
		dark_yellow,
		light_gray,
		dark_gray,
		light_blue,
		light_green,
		light_cyan,
		light_red,
		light_magenta,
		light_yellow,
		white,

		none = -1
	};

public:
	explicit ConsoleTextColor(int color)
		: m_curTextColor(color)
	{
		std::wcout.imbue(std::locale("")); // �̰��� �߰��ϸ� �ȴ�.

		m_oldTextColor = ConsoleTextColor::GetTextColor();
		_SetTextColor(color);
	}
	~ConsoleTextColor()
	{
		_SetTextColor(light_gray);
	}

	static int GetTextColor()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		return csbi.wAttributes;
	}

private:
	void _SetTextColor(int color)
	{
		if (color != none)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		}
	}

private:
	int m_curTextColor;
	int m_oldTextColor;
};
#else
class ConsoleTextColor {
public:
	enum {
		black = 30,
		dark_blue = 34,
		dark_green = 32,
		dark_cyan = 36,
		dark_red = 31,
		dark_magenta = 35,
		dark_yellow = 33,
		light_gray,
		dark_gray,
		light_blue = 34,
		light_green = 32,
		light_cyan = 36,
		light_red = 31,
		light_magenta =35,
		light_yellow =33,
		white = 37,

		default_txt = 39,
		none = -1
	};

public:
	explicit ConsoleTextColor(int color) {

		std::wcout.imbue(std::locale("")); // �̰��� �߰��ϸ� �ȴ�.

		m_oldTextColor = default_txt;
		m_curTextColor = color;
	}

	~ConsoleTextColor() {

		_SetTextColor(light_gray);
	}

	int GetTextColor() {

		return m_curTextColor;
	}

private:
	void _SetTextColor(int color) {

		if (color != none)
		{
			m_curTextColor = color;
		}
	}

private:
	int m_curTextColor;
	int m_oldTextColor;
};

#endif // defined
class ConsoleLogWriter : public ILogWriter {
public:
	virtual ~ConsoleLogWriter() = default;

	virtual void Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg, const tstring& strCompletedLog) override {

		int color;

		switch (level) {
		case LOG_FATAL:
			color = ConsoleTextColor::light_red;
			break;
		case LOG_ERROR:
			color = ConsoleTextColor::light_magenta;
			break;
		case LOG_WARN:
			color = ConsoleTextColor::light_cyan;
			break;
		case LOG_INFO:
			color = ConsoleTextColor::light_yellow;
			break;
		default:
#if defined (_WIN32) || defined(_WIN64)
			color = ConsoleTextColor::GetTextColor();
#else
			color = ConsoleTextColor::default_txt;
#endif
			break;
		}
#if defined (_WIN32) || defined(_WIN64)
		ConsoleTextColor txtColor(color);
		tcout << strCompletedLog << std::flush;
#else
		// 1:m - bright, m - normal
		tcout << _T("\033[") << color << _T(";1m")<< strCompletedLog << std::flush;
#endif
	}
};

class OutputDebugLogWriter : public ILogWriter {

public:
	virtual ~OutputDebugLogWriter() = default;

	virtual void Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg, const tstring& strCompletedLog) override	{

#if defined (_WIN32) || defined(_WIN64)
		OutputDebugString(strCompletedLog.c_str());
#endif
	}
};

class FileLogWriter : public ILogWriter {

public:
	FileLogWriter(const tstring& processName)
		: m_strProcessName(processName) {
	}
	virtual ~FileLogWriter() = default;

	virtual void Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg, const tstring& strCompletedLog) override {

		tstring strFileTime = _ConvertTimeToString(curTime);

		if (m_fs.is_open() == false || m_fs.good() == false || m_strFileTime != strFileTime) {
			m_strFileTime = strFileTime;
			_OpenFile(strFileTime);

			assert(m_fs.is_open());
		}

		m_fs << strCompletedLog;
		m_fs.flush();
	}

private:

	bool _OpenFile(const tstring& strFileTime) {
		//tstring strLogDir = util::FileSystem::GetCurrentPath() + _T("\\log");
		tstring strBaseDir = util::FileSystem::GetCurrentPath();

		if(strBaseDir == _T("")) {

			strBaseDir = _T("/tmp");
		}

		tstring strLogDir = util::StringHelper::Format(_T("%s%slog"), strBaseDir.c_str(), DIR_DELIMETER);

		util::FileSystem::CreateDir(strLogDir);

		tstring strFilePath;
		util::StringHelper::Format(strFilePath, _T("%s%s%s_%s.log"), strLogDir.c_str(), DIR_DELIMETER, m_strProcessName.c_str(), strFileTime.c_str());

		m_fs.close();
		m_fs.open(strFilePath, std::ios::out | std::ios::app);

		if (m_fs.is_open()) {

			std::string strLocaleName = util::FileSystem::GetSystemLocaleName();
			m_fs.imbue(std::locale(strLocaleName.c_str()));
		}
		return m_fs.is_open();
	}

	tstring _ConvertTimeToString(time_t curTime) {
		tstringstream ss;
		struct tm time_info;

		thread_safe_localtime(&curTime, &time_info);
		ss << std::put_time(&time_info, _T("%Y%m%d_%H"));

		return ss.str();
	}

	tofstream	m_fs;
	tstring		m_strFileTime;
	tstring     m_strProcessName;
};

///////////////////////////////////////////////////////////////////////////////////////////
LogMgr::LogMgr()
	: m_pGcd(nullptr)
	, m_LogLevel(LOG_DEBUG) {

}


LogMgr::~LogMgr() {
	Uninit();
}

bool LogMgr::Init(int level, int type) {

	assert(m_pGcd == nullptr);
	m_LogLevel = level;

	// write lock
	std::unique_lock<MUTEX_TYPE> lock(m_gcdMutex);

	m_pGcd = std::make_shared<Gcd>();

	if (m_pGcd->Init() == 0)
	{
		_RegisterLogger(type);
	}

	std::string strLocaleName = util::FileSystem::GetSystemLocaleName();
	tcout.imbue(std::locale(strLocaleName.c_str()));

	return true;
}

void LogMgr::Uninit() {

	if (m_pGcd != nullptr)
	{
		m_pGcd->DispatchSync(&LogMgr::_UnregisterLogger, this);

		// write lock
		std::unique_lock<MUTEX_TYPE> lock(m_gcdMutex);

		m_pGcd->Fini(true);
		m_pGcd = nullptr;
	}
	else
	{
		_UnregisterLogger();
	}
	tcout.imbue(std::locale("C"));
}

void LogMgr::SetLevel(uint16_t level) {

	m_LogLevel.store(level);
}

void LogMgr::RegisterLogWriter(const ILogWriterPtr& pLogWriter) {

	// write lock
	std::unique_lock<MUTEX_TYPE> lock(m_lstWriterMutext);
	m_lstWriter.insert(pLogWriter);
}

void LogMgr::UnregisterLogWriter(const ILogWriterPtr& pLogWriter) {
	// write lock
	std::unique_lock<MUTEX_TYPE> lock(m_lstWriterMutext);
	m_lstWriter.erase(pLogWriter);
}

bool LogMgr::_IsSet(int type, int mask) {

	return ((type & mask) ==  mask);
}

void LogMgr::_RegisterLogger(int type) {

	if (_IsSet(type, LOG_WRITER_CONSOLE)) {

		m_lstWriter.insert(std::make_unique<ConsoleLogWriter>());
	}

	if (_IsSet(type, LOG_WRITER_OUTPUT_DEBUG)) {

		m_lstWriter.insert(std::make_unique<OutputDebugLogWriter>());
	}

	if (_IsSet(type, LOG_WRITER_FILE)) {

		tstring strFName = util::FileSystem::GetFileName();
		if(strFName == _T("")) {

			strFName = _T("unknown");
		}

		tstring::size_type pos = strFName.find(_T("."));

		if (pos != tstring::npos) {

			strFName = strFName.substr(0, pos);
		}

		m_lstWriter.insert(std::make_unique<FileLogWriter>(strFName));
	}
}

int LogMgr::_UnregisterLogger() {

	m_lstWriter.clear();

	return 0;
}

time_t LogMgr::_GetTime() {

	auto now = std::chrono::system_clock::now();
	auto cur_time_t = std::chrono::system_clock::to_time_t(now);

	return cur_time_t;
}

tstring LogMgr::_GetCurrentTimeStr() {

	using namespace std::chrono;

	auto p = system_clock::now();
	auto ms = duration_cast<milliseconds>(p.time_since_epoch());
	auto s = duration_cast<seconds>(ms);
	std::time_t tm_ = s.count();
	struct tm lt;
	//tm *plt = localtime_t(&tm_);
	thread_safe_localtime(&tm_, &lt);

	tchar strTime[100] = { 0, };
	tsnprintf(strTime, 100, _T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
		lt.tm_year + 1900,
		lt.tm_mon + 1,
		lt.tm_mday,
		lt.tm_hour,
		lt.tm_min,
		lt.tm_sec,
		(unsigned)(ms.count() % 1000));
	return strTime;
}

tstring LogMgr::_GetCurrentThreadId() {

	tstringstream ss;

	ss << std::this_thread::get_id();

	return ss.str();
}

tstring LogMgr::_GetLogLevelStr(int level) const {

	switch (level)
	{
	case LOG_DEBUG:
		return _T("DEBUG");
	case LOG_INFO:
		return _T("INFO");
	case LOG_ERROR:
		return _T("ERROR");
	case LOG_FATAL:
		return _T("FATAL");
	case LOG_WARN:
		return _T("WARN");
	default:
		return _T("UNKNOWN");
	}
}

// called logmgr's gcd
void LogMgr::_Evt_Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg) {

	tstringstream ss;
	tstring strFName = strFileName;

	std::size_t pos = strFileName.rfind(DIR_DELIMETER);

	if(pos != tstring::npos) {

		pos = (std::min)(pos + 1, strFileName.size());
		strFName = strFileName.substr(pos);
	}

	ss << strFName << _T("(") << nLine << _T("): [") << _GetLogLevelStr(level) << _T("] [") << strTime << _T("(") << strTid << _T(")] ") << strMsg << _T("\n");

	// read lock
	std::lock_guard<MUTEX_TYPE> lock(m_lstWriterMutext);
	for (auto it = m_lstWriter.begin(); it != m_lstWriter.end(); ++it) {

		(*it)->Write(strFileName, nLine, level, curTime, strTid, strTime, strMsg, ss.str());
	}
}