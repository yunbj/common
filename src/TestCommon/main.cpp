#include <iostream>

#include "TestStringConvertor.h"
#include "TestFileSystem.h"
#include "TestStringHelper.h"


#include <getopt.h>

void help(char *argv)
{
    wprintf(L"Usage : %s options\n", argv);
    wprintf(L"--debug\n--create\n--file [file name]\n--help\n");
}

int main(int argc, char* argv[])
{
    TestStringConvertor sc;
    sc.DoTest();

    TestStringHelper sh;
    sh.DoTest();

    TestFileSystem fs;
    fs.DoTest();

    wprintf(L"Teset Complete....\n");

    return 0;
}
