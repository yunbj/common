#include <base/StringConvertor.h>

#include <codecvt>
#include <locale>

#include <vector>

using namespace grid;
using namespace grid::util;

///////////////////////////////////////////////////////////////////////////////////////////
// multi-byte to wide char
std::wstring StringConvertor::MultiByte2WideChar(const std::string& strSrc) {

    std::locale loc = std::locale("");
    std::string str = strSrc;

	if(str.empty())
    {
        return std::wstring();
    }
    
    const char* psz = str.c_str();
    
    std::mbstate_t state = std::mbstate_t();
    unsigned int len = 1 + std::mbsrtowcs(nullptr, &psz, 0, &state);
    
    std::vector<wchar_t> v(len);
    std::mbsrtowcs(&v[0], &psz, v.size(), &state);
    
    std::wstring wstr;
    wstr.assign(v.data(), v.size() - 1); // -1은 assign에서 \0 이 추가되어 \0을 제거하기 위함이다.
    
    return wstr;
}

std::wstring StringConvertor::MultiByte2WideChar(uint32_t codepage, const std::string& strSrc) {

    return MultiByte2WideChar(strSrc);
}

std::wstring StringConvertor::MultiByte2WideChar(const char * pszSrc) {

    std::string str(pszSrc);

    return MultiByte2WideChar(str);
}

std::wstring StringConvertor::MultiByte2WideChar(const char * pszSrc, size_t len) {

    std::string str(pszSrc, len);

    return MultiByte2WideChar(str);
}

std::wstring StringConvertor::MultiByte2WideChar(uint32_t codepage, const char * pszSrc, size_t len) {

    return MultiByte2WideChar(pszSrc, len);
}
///////////////////////////////////////////////////////////////////////////////////////////
// wide char to multi-byte char
std::string StringConvertor::WideChar2MultiByte(const std::wstring& wstrSrc) {

    std::locale loc = std::locale("");
    std::wstring wstr = wstrSrc;
    
    const wchar_t * pwsz = wstr.c_str();
    
    std::mbstate_t state = std::mbstate_t();
    unsigned int len = 1 + std::wcsrtombs(nullptr, &pwsz, 0, &state);
    
    std::vector<char> v(len);
    std::wcsrtombs(&v[0], &pwsz, v.size(), &state);
    
    std::string str;
    str.assign(v.data(), v.size() - 1);
    
    return str;
}

std::string StringConvertor::WideChar2MultiByte(uint32_t codepage, const std::wstring& wstrSrc) {

    return WideChar2MultiByte(wstrSrc);
}

std::string StringConvertor::WideChar2MultiByte(const wchar_t* pwszSrc) {

    std::wstring wstr(pwszSrc);

    return WideChar2MultiByte(wstr);
}

std::string StringConvertor::WideChar2MultiByte(const wchar_t* pwszSrc, size_t len) {

    std::wstring wstr(pwszSrc, len);

    return WideChar2MultiByte(wstr);
}

std::string StringConvertor::WideChar2MultiByte(uint32_t codepage, const wchar_t * pwszSrc, size_t len) {

    return WideChar2MultiByte(pwszSrc, len);
}

///////////////////////////////////////////////////////////////////////////////////////////
// UTF8 to wide char convert
std::wstring StringConvertor::UTF8ToWideChar(const char * pszUTF8) {

    std::string str(pszUTF8);

    return UTF8ToWideChar(str);
}

std::wstring StringConvertor::UTF8ToWideChar(const char * pszUTF8, size_t len) {

    std::string str(pszUTF8, len);

    return UTF8ToWideChar(str);
}

std::wstring StringConvertor::UTF8ToWideChar(const std::string& strUTF8) {

    std::string utf8 = strUTF8;

    // UTF-8 to wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
	std::wstring wstr = wconv.from_bytes(utf8);

	return wstr;
}

///////////////////////////////////////////////////////////////////////////////////////////
// wide char to UTF8 convert
std::string StringConvertor::WideCharToUTF8(const wchar_t * pwszSrc) {

    std::wstring str(pwszSrc);

    return WideChar2MultiByte(str);
}

std::string StringConvertor::WideCharToUTF8(const wchar_t * pwszSrc, size_t len) {

    std::wstring str(pwszSrc, len);

    return WideChar2MultiByte(str);
}

std::string StringConvertor::WideCharToUTF8(const std::wstring& wstrSrc) {

    // wstring to UTF-8
    std::wstring wstr = wstrSrc;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
	std::string str = wconv.to_bytes(wstr);

	return str;
}

///////////////////////////////////////////////////////////////////////////////////////////
// UTF8 to multi-byte char convert
std::string StringConvertor::UTF8ToMultiBytes(const char * pszUTF8) {

   std::string strUTF8(pszUTF8);

   return UTF8ToMultiBytes(strUTF8);
}

std::string StringConvertor::UTF8ToMultiBytes(const char * pszUTF8, size_t len) {

    std::string strUTF8(pszUTF8, len);

   return UTF8ToMultiBytes(strUTF8);
}

std::string StringConvertor::UTF8ToMultiBytes(const std::string& strUTF8) {

    std::wstring wstr = UTF8ToWideChar(strUTF8);

	return WideChar2MultiByte(wstr);
}

//////////////////////////////////////////////////////////////////////////////l/////////////
// multi-byte char to UTF8 convert
std::string StringConvertor::MultiBytesToUTF8(const char * pszSrc) {

    std::string str(pszSrc);

    return MultiBytesToUTF8(str);
}

std::string StringConvertor::MultiBytesToUTF8(const char * pszSrc, size_t len) {

    std::string str(pszSrc, len);

    return MultiBytesToUTF8(str);
}

std::string StringConvertor::MultiBytesToUTF8(const std::string& strSrc) {

    std::wstring wstr = MultiByte2WideChar(strSrc);

	return WideCharToUTF8(wstr);
}
