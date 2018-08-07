#pragma once


#include <cstdint>
#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>

#if defined (_WIN32) || defined(_WIN64)
	#include <tchar.h>
	#define DIR_DELIMETER       _T("\\")

	// INT64_FMT를 mongoose 중복확인 없이 사용중이여서 GRID_ 추가
	#define GRID_INT64_FMT           "I64d"
	#define GRID_UINT64_FMT          "I64u"
#else
	#ifdef UNICODE
		#define _T(s)	L##s
	#else
		#define	_T(s)	s
	#endif// UNICODE

	#define DIR_DELIMETER       _T("/")

	// PRId64
	#define __STDC_FORMAT_MACROS
    #include <inttypes.h>
    #define GRID_INT64_FMT         PRId64
    #define GRID_UINT64_FMT        PRIu64

	#include <stdarg.h>
    #include <wchar.h>
#endif

namespace grid {

	#if defined(UNICODE)
		#define	tstring			std::wstring
		#define	tcout			std::wcout
		#define tcerr			std::wcerr
		#define	tcout			std::wcout
		#define to_tstring		std::to_wstring
		#define tchar			wchar_t
		#define tstringstream	std::wstringstream
		#define tistringstream	std::wistringstream
		#define tostringstream	std::wostringstream

		#define tifstream		std::wifstream
		#define tofstream		std::wofstream
		#define tfstream		std::wfstream

		#define vstprintf		std::vswprintf

#if defined (_WIN32) || defined(_WIN64)
		#define tsnprintf		_sntprintf_s
		#define tstrcpy			wcscpy_s
		#define tstrncpy		wcsncpy_s
		#define tfopen			_wfopen_s
#else
        #define tsnprintf       swprintf
        #define tstrcpy			wcscpy
		#define tstrncpy		wcsncpy
		// 만약 tfopen을 사용하고 있다면 path를 multibyte로 변경해서 사용해야한다.
        // linux에는 wide형 fopen이 없다.
		#define tfopen			fopen
#endif
		#define tstrchr			wcschr
        #define tstrlen			wcslen


		#define WIDEN2(x)		L##x
		#define WIDEN(x)		WIDEN2(x)
		#define __WFILE__		WIDEN(__FILE__)
		#define __TFILE__		__WFILE__
	#else
		#define	tstring			std::string
		#define	tcout			std::cout
		#define tcerr			std::cerr
		#define to_tstring		std::to_string
		#define	tchar			char
		#define tstringstream	std::stringstream
		#define tistringstream	std::istringstream
		#define tostringstream	std::ostringstream

		#define tifstream		std::ifstream
		#define tofstream		std::ofstream
		#define tfstream		std::fstream

		#define vstprintf		std::vsnprintf
		#define tsnprintf		snprintf

		#define tstrchr			strchr
		#define tstrcpy			strcpy
		#define tstrncpy		strncpy
		#define tstrlen			strlen

#if defined (_WIN32) || defined(_WIN64)
		#define tfopen			fopen_s
#else
        #define tfopen          fopen
#endif

		#define __TFILE__		__FILE__
	#endif


	//typedef uint64_t Timestamp;
	using Timestamp = uint64_t;
	using EncoderId = tstring;

	//using ChannelId = tstring;
	//using QualityId = tstring;
}
