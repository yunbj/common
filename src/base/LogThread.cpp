//
// Created by lacrimas on 18. 11. 19.
//

#include <base/LogThread.h>

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>  // put_time
#include <stdarg.h>
#include <iostream>
#include <algorithm>
#include <thread>

#include <base/Base.h>
#include <base/StringHelper.h>
#include <base/FileSystem.h>

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		: _curTextColor(color)
	{
		std::wcout.imbue(std::locale(""));

		_oldTextColor = ConsoleTextColor::GetTextColor();
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
	int _curTextColor;
	int _oldTextColor;
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

        std::wcout.imbue(std::locale(""));

        _oldTextColor = default_txt;
        _curTextColor = color;
    }

    ~ConsoleTextColor() {

        _SetTextColor(light_gray);
    }

    int GetTextColor() {

        return _curTextColor;
    }

private:
    void _SetTextColor(int color) {

        if (color != none)
        {
            _curTextColor = color;
        }
    }

private:
    int _curTextColor;
    int _oldTextColor;
};

#endif // defined
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ConsoleLogWriter : public ILogWriter {


public:
    ConsoleLogWriter(MUTEX_TYPE& mtxConsole)
            : _mtxConsole(mtxConsole) {

    }
    virtual ~ConsoleLogWriter() = default;

    virtual void Write(const tstring& strFileName,
                       int nLine,
                       int level,
                       std::time_t curTime,
                       const tstring& strTid,
                       const tstring& strTime,
                       const tstring& strMsg,
                       const tstring& strCompletedLog) override {

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

        std::unique_lock<MUTEX_TYPE> lock(_mtxConsole);
#if defined (_WIN32) || defined(_WIN64)
        ConsoleTextColor txtColor(color);
		tcout << strCompletedLog << std::flush;
#else
        // 1:m - bright, m - normal
        tcout << _T("\033[") << color << _T(";1m")<< strCompletedLog << std::flush;
#endif
    }

private:
    MUTEX_TYPE& _mtxConsole;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OutputDebugLogWriter : public ILogWriter {

public:
    OutputDebugLogWriter(MUTEX_TYPE& mtxConsole)
            : _mtxConsole(mtxConsole)
    {
    }
    virtual ~OutputDebugLogWriter() = default;

    virtual void Write(const tstring& strFileName,
                       int nLine,
                       int level,
                       std::time_t curTime,
                       const tstring& strTid,
                       const tstring& strTime,
                       const tstring& strMsg,
                       const tstring& strCompletedLog) override	{

        std::unique_lock<MUTEX_TYPE> lock(_mtxConsole);

#if defined (_WIN32) || defined(_WIN64)
        OutputDebugString(strCompletedLog.c_str());
#endif
    }

private:
    MUTEX_TYPE &    _mtxConsole;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FileLogWriter : public ILogWriter {

public:
    FileLogWriter(const tstring& processName, int idx)
            : _processName(processName)
            , _idx(idx) {
    }
    virtual ~FileLogWriter() = default;

    virtual void Write(const tstring& strFileName,
                       int nLine,
                       int level,
                       std::time_t curTime,
                       const tstring& strTid,
                       const tstring& strTime,
                       const tstring& strMsg,
                       const tstring& strCompletedLog) override {

        tstring fileTime = _ConvertTimeToString(curTime);

        if (_fs.is_open() == false || _fs.good() == false || _fileTime != fileTime) {

            _fileTime = fileTime;
            _OpenFile(fileTime);

            assert(_fs.is_open());
        }

        _fs << strCompletedLog;
        _fs.flush();
    }

private:

    bool _OpenFile(const tstring& fileTime) {

        tstring baseDir = util::FileSystem::GetCurrentPath();

        if(baseDir == _T("")) {

            baseDir = _T("/tmp");
        }

        tstring logDir = util::StringHelper::Format(_T("%s%slog"), baseDir.c_str(), DIR_DELIMETER);

        util::FileSystem::CreateDir(logDir);

        tstring filePath;
        util::StringHelper::Format(filePath, _T("%s%s%s_%s_%d.log"), logDir.c_str(), DIR_DELIMETER, _processName.c_str(), fileTime.c_str(), _idx);

        _fs.close();
        _fs.open(filePath, std::ios::out | std::ios::app);

        if (_fs.is_open()) {

            std::string strLocaleName = util::FileSystem::GetSystemLocaleName();
            _fs.imbue(std::locale(strLocaleName.c_str()));
        }

        return _fs.is_open();
    }

    tstring _ConvertTimeToString(std::time_t curTime) {
        tstringstream ss;
        struct tm time_info;

        thread_safe_localtime(&curTime, &time_info);
        ss << std::put_time(&time_info, _T("%Y%m%d_%H"));

        return ss.str();
    }

    tofstream	_fs;
    tstring		_fileTime;
    tstring     _processName;
    int         _idx;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LogThread::LogThread(MUTEX_TYPE& mtxConsole, int idx)
: _mtxConsole(mtxConsole)
, _idx(idx) {

}

LogThread::~LogThread() {

}

bool LogThread::Init(int type) {

    _gcd = std::make_shared<Gcd>();

    if (_gcd->Init() == 0)
    {
        _gcd->DispatchSync([self=this, type] {

            self->_threadId = self->_GetCurrentThreadId();
            self->_RegisterLogger(type);

            return 0;
        });

        return true;
    }

    return false;
}

void LogThread::Uninit() {

    if (_gcd != nullptr)
    {
        _gcd->DispatchSync([self=this]{


            self->_UnregisterLogger();
            return 0;
        });

        _gcd->Fini(true);
        _gcd = nullptr;
    }
    else
    {
        _UnregisterLogger();
    }
}

void LogThread::RegisterLogWriter(const ILogWriterPtr& writer) {
    _gcd->DispatchAsync([self=this, writer]{

        self->_writers.insert(writer);
    });
}

void LogThread::UnregisterLogWriter(const ILogWriterPtr& writer) {
    _gcd->DispatchAsync([self=this, writer]{

        self->_writers.erase(writer);
    });
}

void LogThread::Write(uint16_t seq,
                      const tstring& fileName,
                      int line,
                      int level,
                      std::time_t curTime,
                      const tstring& tid,
                      const tstring& writeTime,
                      const tstring& msg) {

    _gcd->DispatchAsync([self=this, seq, fileName, line, level, curTime, tid, writeTime, msg]{

        self->_WriteLog(seq, fileName, line, level, curTime, tid, writeTime, msg);
    });
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void LogThread::_RegisterLogger(int type) {

    if (_IsSet(type, LOG_WRITER_CONSOLE)) {

        _writers.insert(std::make_unique<ConsoleLogWriter>(_mtxConsole));
    }

    if (_IsSet(type, LOG_WRITER_OUTPUT_DEBUG)) {

        _writers.insert(std::make_unique<OutputDebugLogWriter>(_mtxConsole));
    }

    if (_IsSet(type, LOG_WRITER_FILE)) {

        tstring procName = util::FileSystem::GetFileName();

        if(procName == _T("")) {

            procName = _T("unknown");
        }

        tstring::size_type pos = procName.find(_T("."));

        if (pos != tstring::npos) {

            procName = procName.substr(0, pos);
        }

        _writers.insert(std::make_unique<FileLogWriter>(procName, _idx));
    }
}

void LogThread::_UnregisterLogger() {

    _writers.clear();
}

bool LogThread::_IsSet(int type, int mask) {

    return ((type & mask) ==  mask);
}

tstring LogThread::_GetCurrentThreadId() {

    tstringstream ss;

    ss << std::this_thread::get_id();

    return ss.str();
}

tstring LogThread::_GetLogLevelStr(int level) const {

    switch (level)
    {
        case LOG_DEBUG:
            return _T("[DEBUG]");
        case LOG_INFO:
            return _T("[INFO] ");
        case LOG_ERROR:
            return _T("[ERROR]");
        case LOG_FATAL:
            return _T("[FATAL]");
        case LOG_WARN:
            return _T("[WARN] ");
        default:
            return _T("[UNKNOWN]");
    }
}


void LogThread::_WriteLog(uint16_t seq,
                          const tstring& fileName,
                          int line,
                          int level,
                          std::time_t curTime,
                          const tstring& tid,
                          const tstring& writeTime,
                          const tstring& msg) {

    tstringstream ss;
    tstring fName = fileName;

    std::size_t pos = fName.rfind(DIR_DELIMETER);

    if(pos != tstring::npos) {

        pos = (std::min)(pos + 1, fileName.size());
        fName = fileName.substr(pos);
    }

    tstring fileInfo = util::StringHelper::Format(_T("%s(%u)"), fName.c_str(), line);
    tstring log;

    util::StringHelper::Format(log, "%-30s : %5s [%s](%05u) (%s) %s\n",
                               fileInfo.c_str(),
                               _GetLogLevelStr(level).c_str(),
                               writeTime.c_str(),
                               seq,
                               tid.c_str(),
                               msg.c_str());

    for (auto it = _writers.begin(); it != _writers.end(); ++it) {

        (*it)->Write(fileName, line, level, curTime, tid, writeTime, msg, log);
    }
}