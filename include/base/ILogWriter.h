#pragma once

#include <base/Base.h>

namespace grid {

	namespace log {

		class ILogWriter {
		public:
			virtual ~ILogWriter() = default;
			virtual void Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg, const tstring& strCompletedLog) = 0;

		};

	} // end namespace log
} // end namespace grid
