#include "TestStringConvertor.h"

#include <base/StringConvertor.h>
#include <assert.h>

using namespace grid;
using namespace grid::util;

using namespace std;

TestStringConvertor::TestStringConvertor()
{
    //ctor
}

TestStringConvertor::~TestStringConvertor()
{
    //dtor
}

void TestStringConvertor::DoTest()
{

// for hangul print.
#if defined(WIN32) || defined(_WIN64) || defined(__APPLE__)
    auto old_local = setlocale(LC_ALL, "ko_KR.UTF-8");
#else
    auto old_local = std::setlocale(LC_ALL,"");
#endif

    wprintf(L"\n===== [ start test string convertor ] =====\n");

	string utf8str = u8"가나다라마바사....abcdefg한";
	string mbstr = "가나다라마바사....abcdefg한";
    wstring wstr = L"가나다라마바사....abcdefg한";

	// check wstr to multi-byte str
	string mbstr2 = StringConvertor::WideChar2MultiByte(wstr);
	assert((mbstr == mbstr2));
	wprintf(L"[wstr_to_str] mbstr2 : %s\n", mbstr2.c_str());

	// reverse
	// check multi-byte str to wstr
	wstring wstr2 = StringConvertor::MultiByte2WideChar(mbstr2);
	assert(wstr == wstr2);
	wprintf(L"[str_to_wstr] wstr2 : %ls\n", wstr2.c_str());

	// check utf8 str to multi-byte str
	string mbstr3 = StringConvertor::UTF8ToMultiBytes(utf8str);
	assert(mbstr == mbstr3);
	wprintf(L"[utf8_to_str] mbstr3 : %s\n", mbstr3.c_str());
	// reverse
	// check multi-byte str to utf8
	string utf8str2 = StringConvertor::MultiBytesToUTF8(mbstr3);
	assert(utf8str == utf8str2);
	wprintf(L"[str_to_utf8] utf8str2 : %s\n", utf8str2.c_str());

	// check utf8 str to wide str
	wstring wstr3 = StringConvertor::UTF8ToWideChar(utf8str);
	assert(wstr == wstr3);
	wprintf(L"[utf8_to_wstr] wstr3 : %ls\n", wstr3.c_str());

	// check wide str to utf8
	string utf8str3 = StringConvertor::WideCharToUTF8(wstr3);
	assert(utf8str == utf8str3);
	wprintf(L"[wstr_to_utf8] utf8str3 : %s\n", utf8str3.c_str());

#if defined(WIN32) || defined(_WIN64) || defined(__APPLE__)
    setlocale(LC_ALL, old_local);
#else
    std::setlocale(LC_ALL,old_local);
#endif

}
