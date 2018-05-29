#pragma once

#include <base/Base.h>

namespace grid
{
	namespace util
	{
		class StringHelper
		{
		public:
			static constexpr const tchar * STR_TRIM_SPACE = _T(" \r\n\t");

			static void LTrim(tstring& str, const tstring& drop = STR_TRIM_SPACE);
			static void RTrim(tstring& str, const tstring& drop = STR_TRIM_SPACE);
			static void Trim(tstring& str, const tstring& drop = STR_TRIM_SPACE) ;

			static void ToUpper(tstring& str);
			static void ToLower(tstring& str);

			static void Format(tstring& strResult, const tchar * pszFmt, ...);
			static tstring Format(const tchar * pszFmt, ...);
			static void AppendFormat(tstring& strResult, const tchar * pszFmt, ...);

			static void FormatV(tstring& strResult, const tchar * pszFmt, va_list args);
			static void AppendFormatV(tstring& strResult, const tchar * pszFmt, va_list args);
		};
	}
}

#ifndef APPEND_FORMAT
#define APPEND_FORMAT		grid::util::StringHelper::AppendFormat
#endif

#ifndef FORMAT
#define FORMAT				grid::util::StringHelper::Format
#endif