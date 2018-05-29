#include <base/StringHelper.h>

#include <algorithm>
#include <functional>
#include <cctype>

#include <vector>
#include <cstdarg>
 
namespace grid
{
	namespace util
	{

		void StringHelper::RTrim(tstring& str, const tstring& drop /*= STR_TRIM_SPACE */)
		{
			str.erase(str.find_last_not_of(drop) + 1);
		}

		void StringHelper::LTrim(tstring& str, const tstring& drop /*= STR_TRIM_SPACE */)
		{
			str.erase(0, str.find_first_not_of(drop));
		}

		void StringHelper::Trim(tstring& str, const tstring& drop /*= STR_TRIM_SPACE */)
		{
			LTrim(str, drop);
			return RTrim(str, drop);
		}

		void StringHelper::ToUpper(tstring& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		}

		void StringHelper::ToLower(tstring& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		}

		void StringHelper::Format(tstring& strResult, const tchar * pszFmt, ...)
		{
			va_list args;
			va_start(args, pszFmt);

			FormatV(strResult, pszFmt, args);

			va_end(args);
		}

		tstring StringHelper::Format(const tchar * pszFmt, ...)
		{
			tstring strResult;

			va_list args;
			va_start(args, pszFmt);

			FormatV(strResult, pszFmt, args);

			va_end(args);

			return strResult;
		}

		void StringHelper::FormatV(tstring& strResult, const tchar * pszFmt, va_list args)
		{
            // linux에서는 vstprintf 호출하면 args의 offset값이 변경되어, 실제 값을 넣는 vstprintf 호출 시
            // 다른 값이 들어가 버리기 때문에 사용전에 복사를 해둬야 한다.
            va_list args_copy;
            va_copy(args_copy, args);

			int len = vstprintf(nullptr, 0, pszFmt, args);

			if (len <= 0)
			{
                va_end(args_copy);
				return;
			}

			std::vector<tchar> buf(len + 1); // + 1 is for the terminating null character.
			vstprintf(buf.data(), buf.size(), pszFmt, args_copy);

			if (len > 0)
			{
				strResult.assign(buf.begin(), buf.end() - 1); // -1 is for remove null character.
			}

			va_end(args_copy);
		}

		void StringHelper::AppendFormatV(tstring& strResult, const tchar * pszFmt, va_list args)
		{
            // linux에서는 vstprintf 호출하면 args의 offset값이 변경되어, 실제 값을 넣는 vstprintf 호출 시
            // 다른 값이 들어가 버리기 때문에 사용전에 복사를 해둬야 한다.
            va_list args_copy;
            va_copy(args_copy, args);

			int len = vstprintf(nullptr, 0, pszFmt, args);

			if (len <= 0)
			{
                va_end(args_copy);
				return;
			}

			std::vector<tchar> buf(len + 1); // + 1 is for the terminating null character.
			len = vstprintf(buf.data(), buf.size(), pszFmt, args_copy);

			if (len > 0)
			{
				strResult.append(buf.begin(), buf.end() - 1); // -1 is for remove null character.
			}

			va_end(args_copy);
		}

		void StringHelper::AppendFormat(tstring& strResult, const tchar * pszFmt, ...)
		{
			va_list args;
			va_start(args, pszFmt);

			AppendFormatV(strResult, pszFmt, args);

			va_end(args);
		}
	}
}
