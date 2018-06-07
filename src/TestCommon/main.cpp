#include <iostream>

#include <base/LogMgr.h>

#include <ctime>
#include <thread>
#include <chrono>

#if !defined(_WIN32)
#include "TestStringConvertor.h"
#endif

#include "TestFileSystem.h"
#include "TestStringHelper.h"
#include "TestGcd.h"

using namespace grid;
using namespace std::chrono_literals;

void help(char *argv)
{
    wprintf(L"Usage : %s options\n", argv);
    wprintf(L"--debug\n--create\n--file [file name]\n--help\n");
}

int main(int argc, char* argv[])
{
	INIT_LOG_MGR(log::LOG_LEVEL::LOG_DEBUG, log::LOG_WRITER_TYPE::LOG_WRITER_ALL);

#if !defined(_WIN32)
	TestStringConvertor sc;
	sc.DoTest();
#endif

	TestStringHelper sh;
	sh.DoTest();

	TestFileSystem fs;
	fs.DoTest();

	TestGcd tg;
	tg.DoTest();


	/// test log print time
	for (auto i = 0; i < 10; ++i)
	{
		std::this_thread::sleep_for(100ms);

		DEBUG_LOG(_T("sleep 100ms."));
	}

	//getchar();

	UNINIT_LOG_MGR();


    std::cout << "Test set Complete....\n";

//	getchar();

    return 0;
}
