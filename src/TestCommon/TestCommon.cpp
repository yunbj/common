// TestCommon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <base/LogMgr.h>

using namespace grid;

int main()
{
	INIT_LOG_MGR(log::LOG_LEVEL::LOG_DEBUG, log::LOG_WRITER_TYPE::LOG_WRITER_ALL);

	DEBUG_LOG(_T("hello world"));

    return 0;
}

