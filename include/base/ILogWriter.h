#pragma once

#include <base/Base.h>

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

		class ILogWriter {
		public:
			virtual ~ILogWriter() = default;
			virtual void Write(const tstring& strFileName, int nLine, int level, time_t curTime, const tstring& strTid, const tstring& strTime, const tstring& strMsg, const tstring& strCompletedLog) = 0;

		};

	} // end namespace log
} // end namespace grid
