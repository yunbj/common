//
// Created by lacrimas on 18. 11. 19.
//

#ifndef COMMON_LOGTHREAD_H
#define COMMON_LOGTHREAD_H

#endif //COMMON_LOGTHREAD_H

#include <base/Base.h>
#include <base/Gcd.h>
#include <base/ILogWriter.h>
#include <time.h>	// localtime
#include <memory>
#include <set>
#include <iomanip>
#include <shared_mutex>
#include "StringHelper.h"

namespace grid
{
    namespace log
    {
        using MUTEX_TYPE = std::mutex;

        class ILogWriter;
        class LogThread
        {
            using ILogWriterPtr = std::shared_ptr<ILogWriter>;
            using LogWriterList = std::set<ILogWriterPtr>;
            using GcdPtr = std::shared_ptr<grid::Gcd>;

        public:
            LogThread(MUTEX_TYPE& mtxConsole, int idx);
            ~LogThread();

            bool Init(int type);
            void Uninit();

            void RegisterLogWriter(const ILogWriterPtr& writer);
            void UnregisterLogWriter(const ILogWriterPtr& pLogWriter);

            void Write(uint16_t seq,
                       const tstring& fileName,
                       int line,
                       int level,
                       std::time_t curTime,
                       const tstring& tid,
                       const tstring& writeTime,
                       const tstring& msg);


            template<class Fn, class...Args>
            int DispatchAsync(Fn &&fn, Args &&... args) {

                return _gcd->DispatchAsync(std::forward<Fn>(fn), std::forward<Args>(args)...);
            }

            template<class Fn, class...Args>
            decltype(auto) DispatchSync(Fn &&fn, Args &&... args) {

                return _gcd->DispatchSync(std::forward<Fn>(fn), std::forward<Args>(args)...);
            };

        private:
            void        _RegisterLogger(int type);
            void        _UnregisterLogger();
            tstring     _GetCurrentThreadId();
            tstring     _GetLogLevelStr(int level) const;
            bool        _IsSet(int type, int mask);

            void _WriteLog(uint16_t seq,
                           const tstring& fileName,
                           int line,
                           int level,
                           std::time_t curTime,
                           const tstring& tid,
                           const tstring& writeTime,
                           const tstring& msg);


            GcdPtr              _gcd;
            MUTEX_TYPE&		    _mtxConsole;
            tstring             _threadId;
            int                 _idx = {0};
            LogWriterList		_writers;

        };
    }
}