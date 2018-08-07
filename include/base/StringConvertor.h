#pragma once

#include <base/Base.h>

#ifndef GRID_STR_CONV
#define GRID_STR_CONV

#if defined(WIN32) || defined(_WIN64)
#define GRID_W2A(str)					grid::util::StringConvertor::WideChar2MultiByte(str)
#define GRID_A2W(str)					grid::util::StringConvertor::MultiByte2WideChar(str)

#define GRID_WSZ2ASTR(sz, len)			grid::util::StringConvertor::WideChar2MultiByte(sz, len)
#define GRID_SZ2WSTR(sz, len)			grid::util::StringConvertor::MultiByte2WideChar(sz, len)

#define GRID_WSZ2WSTR(sz, len)			std::wstring(sz, len)
#define GRID_SZ2STR(sz, len)			std::string(sz, len)

// utf8 convert for unicode
#define GRID_UTF8_TO_WSTR(str)			grid::util::StringConvertor::UTF8ToWideChar(str)
#define GRID_UTF8SZ_TO_WSTR(sz, len)	grid::util::StringConvertor::UTF8ToWideChar(sz, len)

#define GRID_WSTR_TO_UTF8(str)			grid::util::StringConvertor::WideCharToUTF8(str)
#define GRID_WSZ_TO_UTF8(sz, len)		grid::util::StringConvertor::WideCharToUTF8(sz, len)


// utf8 convert for ansi
#define GRID_UTF8_TO_STR(str)			grid::util::StringConvertor::UTF8ToMultiBytes(str)
#define GRID_UTF8SZ_TO_STR(sz, len)		grid::util::StringConvertor::UTF8ToMultiBytes(sz, len)

#define GRID_STR_TO_UTF8(str)			grid::util::StringConvertor::MultiBytesToUTF8(str)
#define GRID_SZ_TO_UTF8(sz, len)		grid::util::StringConvertor::MultiBytesToUTF8(CP_UTF8, sz, len)

#else // if not WIN32
#define GRID_W2A(str)                   grid::util::StringConvertor::WideChar2MultiByte(str)
#define GRID_A2W(str)                   grid::util::StringConvertor::MultiByte2WideChar(str)

#define GRID_WSZ2ASTR(sz, len)			grid::util::StringConvertor::WideChar2MultiByte(sz, len)
#define GRID_SZ2WSTR(sz, len)			grid::util::StringConvertor::MultiByte2WideChar(sz, len)

#define GRID_WSZ2WSTR(sz, len)			std::wstring(sz, len)
#define GRID_SZ2STR(sz, len)			std::string(sz, len)

// utf8 convert for unicode
#define GRID_UTF8_TO_WSTR(str)          grid::util::StringConvertor::UTF8ToWideChar(str)
#define GRID_UTF8SZ_TO_WSTR(sz, len)	grid::util::StringConvertor::UTF8ToWideChar(sz, len)

#define GRID_WSTR_TO_UTF8_STR(str)      grid::util::StringConvertor::WideCharToUTF8(str)
#define GRID_WSZ_TO_UTF8_STR(sz, len)	grid::util::StringConvertor::WideCharToUTF8(sz, len)

// utf8 convert for ansi
#define GRID_UTF8_TO_STR(str)           grid::util::StringConvertor::UTF8ToMultiBytes(str)
#define GRID_UTF8SZ_TO_STR(sz, len)		grid::util::StringConvertor::UTF8ToMultiBytes(sz, len)

#define GRID_STR_TO_UTF8(str)           grid::util::StringConvertor::MultiBytesToUTF8(str)
#define GRID_SZ_TO_UTF8(sz, len)		grid::util::StringConvertor::MultiBytesToUTF8(CP_UTF8, sz, len)

#endif // WIN32

#if defined(UNICODE) || defined(_UNICODE)
#define GRID_T2A		GRID_W2A
#define GRID_T2W
#define GRID_A2T		GRID_A2W
#define GRID_W2T

#define GRID_TSZ2ASTR	GRID_WSZ2ASTR
#define GRID_TSZ2WSTR	GRID_WSZ2WSTR
#define GRID_SZ2TSTR	GRID_SZ2WSTR
#define GRID_WSZ2TSTR	GRID_WSZ2WSTR

#define GRID_TSTR_TO_UTF8	GRID_WSTR_TO_UTF8
#define GRID_UTF8_TO_TSTR	GRID_UTF8_TO_WSTR

#define GRID_UTF8SZ_TO_TSTR  GRID_UTF8SZ_TO_WSTR

#else
#define GRID_T2A
#define GRID_T2W		GRID_A2W
#define GRID_A2T
#define GRID_W2T		GRID_W2A

#define GRID_TSZ2ASTR	GRID_SZ2STR
#define GRID_TSZ2WSTR	GRID_SZ2WSTR
#define GRID_SZ2TSTR	GRID_SZ2STR
#define GRID_WSZ2TSTR	GRID_WSZ2ASTR

#define GRID_TSTR_TO_UTF8	GRID_STR_TO_UTF8
#define GRID_UTF8_TO_TSTR	GRID_UTF8_TO_STR

#define GRID_UTF8SZ_TO_TSTR  GRID_UTF8SZ_TO_STR

#endif // UNICODE
#endif // GRID_STR_CONV


namespace grid {
	namespace util	{

		class StringConvertor {

		public:
            /*
				@param pszSrc : pszSrc must null-terminate string.
			*/
			static std::wstring MultiByte2WideChar(const char * pszSrc);
			static std::wstring MultiByte2WideChar(const char * pszSrc, size_t len);
			static std::wstring MultiByte2WideChar(const std::string& strSrc);
			static std::wstring MultiByte2WideChar(uint32_t codepage, const char * pszSrc, size_t len);
			static std::wstring MultiByte2WideChar(uint32_t codepage, const std::string& strSrc);

			/*
				@param pszSrc : pwszSrc must null-terminate string.
			*/
			static std::string WideChar2MultiByte(const wchar_t * pwszSrc);
			static std::string WideChar2MultiByte(const wchar_t * pwszSrc, size_t len);
			static std::string WideChar2MultiByte(const std::wstring& wstrSrc);
			static std::string WideChar2MultiByte(uint32_t codepage, const std::wstring& wstrSrc);
			static std::string WideChar2MultiByte(uint32_t codepage, const wchar_t * pwszSrc, size_t len);

			/*
				@param pszSrc : pszUTF8 must null-terminate string.
			*/
			static std::wstring UTF8ToWideChar(const char * pszUTF8);
			static std::wstring UTF8ToWideChar(const char * pszUTF8, size_t len);
			static std::wstring UTF8ToWideChar(const std::string& strUTF8);

			static std::string WideCharToUTF8(const wchar_t * pwszSrc);
			static std::string WideCharToUTF8(const wchar_t * pwszSrc, size_t len);
			static std::string WideCharToUTF8(const std::wstring& wstrSrc);

			static std::string UTF8ToMultiBytes(const char * pszUTF8);
			static std::string UTF8ToMultiBytes(const char * pszUTF8, size_t len);
			static std::string UTF8ToMultiBytes(const std::string& strUTF8);

			static std::string MultiBytesToUTF8(const char * pszSrc);
			static std::string MultiBytesToUTF8(const char * pszSrc, size_t len);
			static std::string MultiBytesToUTF8(const std::string& pszSrc);
		};
	}
}

