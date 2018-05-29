#include "TestStringHelper.h"

#include <base/Base.h>
#include <base/StringHelper.h>

#include <assert.h>

using namespace std;
using namespace nova;
using namespace util;

TestStringHelper::TestStringHelper()
{
    //ctor
}

TestStringHelper::~TestStringHelper()
{
    //dtor
}

void TestStringHelper::DoTest()
{
    wprintf(L"\n===== [ start test string helper ] =====\n");
    {
        // check format string
        tstring strOrg = _T("default string.");
        tstring strAdd = _T("added string.");
        tstring strNew = FORMAT(_T("%s%s"), strOrg.c_str(), strAdd.c_str());
        tstring strResult = strOrg + strAdd;

        assert(strNew == strResult);

        wprintf(L"new : %s, expected result : %s\n", strNew.c_str(), strResult.c_str());
    }
    {
        tstring strOrg = _T("default string.");
        tstring strNew = strOrg;
        tstring strAppend = _T("append string.");

        APPEND_FORMAT(strNew, _T("%s"), strAppend.c_str());
        tstring strResult = strOrg + strAppend;

        wprintf(L"new : %s, expected result : %s\n", strNew.c_str(), strResult.c_str());

        assert(strNew == strResult);
    }
    {
        tstring strOrg = _T("./ts");
        tstring strTrim = strOrg;
        util::StringHelper::LTrim(strTrim, _T(". /"));
        assert(strTrim == _T("ts"));

        wprintf(L"org : %s, trim : %s\n", strOrg.c_str(), strTrim.c_str());


        util::StringHelper::LTrim(strTrim, _T(". /"));
        assert(strTrim == _T("ts"));
        wprintf(L"org : %s, trim : %s\n", strOrg.c_str(), strTrim.c_str());
    }

}
