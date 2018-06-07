#include "TestStringHelper.h"

#include <base/Base.h>
#include <base/StringHelper.h>

#include <assert.h>

using namespace std;
using namespace grid;
using namespace util;

#if !defined(trpintf)
#if defined(UNICODE)
#define tprintf	wprintf
#else // defined(UNICODE)
#define tprintf	printf
#endif // defined(UNICODE)
#endif // !defined(trpintf)


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
	tprintf(_T("\n===== [ start test string helper ] =====\n"));
    {
        // check format string
        tstring strOrg = _T("default string.");
        tstring strAdd = _T("added string.");
        tstring strNew = FORMAT(_T("%s%s"), strOrg.c_str(), strAdd.c_str());
        tstring strResult = strOrg + strAdd;

        assert(strNew == strResult);

		tprintf(_T("new : %s, expected result : %s\n"), strNew.c_str(), strResult.c_str());
    }
    {
        tstring strOrg = _T("default string.");
        tstring strNew = strOrg;
        tstring strAppend = _T("append string.");

        APPEND_FORMAT(strNew, _T("%s"), strAppend.c_str());
        tstring strResult = strOrg + strAppend;

		tprintf(_T("new : %s, expected result : %s\n"), strNew.c_str(), strResult.c_str());

        assert(strNew == strResult);
    }
    {
        tstring strOrg = _T("./ts");
        tstring strTrim = strOrg;
        util::StringHelper::LTrim(strTrim, _T(". /"));
        assert(strTrim == _T("ts"));

		tprintf(_T("org : %s, trim : %s\n"), strOrg.c_str(), strTrim.c_str());


        util::StringHelper::LTrim(strTrim, _T(". /"));
        assert(strTrim == _T("ts"));
		tprintf(_T("org : %s, trim : %s\n"), strOrg.c_str(), strTrim.c_str());
    }

}
