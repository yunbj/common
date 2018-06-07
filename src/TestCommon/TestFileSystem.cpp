#include "TestFileSystem.h"

#include <base/FileSystem.h>
#include <base/StringHelper.h>

//#include <unistd.h> // getpid()
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <assert.h>

#if !defined(__APPLE__)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

using namespace std;
using namespace grid;
using namespace grid::util;

#if !defined(trpintf)
#if defined(UNICODE)
#define tprintf	wprintf
#else // defined(UNICODE)
#define tprintf	printf
#endif // defined(UNICODE)
#endif // !defined(trpintf)


static void print_dir(string path)
{
#if !defined(__APPLE__)
	tprintf(_T("===== [ dir list ] =====\n"));

    for(auto& p: fs::recursive_directory_iterator(path))
    {
        if(fs::is_directory(p))
        {
			tprintf(_T("%s\n"), p.path().string().c_str());
        }
    }
	tprintf(_T("========================\n"));
#endif
}

static bool isDirExist(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}


TestFileSystem::TestFileSystem()
{
    //ctor
}

TestFileSystem::~TestFileSystem()
{
    //dtor
}

void TestFileSystem::DoTest()
{
    tprintf(_T("\n===== [ start test file system ] =====\n"));

    string curPath = FileSystem::GetCurrentPath();

	tprintf(_T("GetCurrentPath() : %s\n"), FileSystem::GetCurrentPath().c_str());
	tprintf(_T("GetFileName() : %s\n"), FileSystem::GetFileName().c_str());
	tprintf(_T("GetCurrentProcessId() : %u\n"), FileSystem::GetCurrentProcessId());
	tprintf(_T("GetParentProcessId() : %u\n"), FileSystem::GetParentProcessId());
	tprintf(_T("GetProcessPath(parent) : %s\n"), FileSystem::GetProcessPath(FileSystem::GetParentProcessId()).c_str() );

    string newPath  = curPath + "/tmp/a";
	tprintf(_T("CreateDir(%s, ret : %d)\n"), newPath.c_str(), FileSystem::CreateDir(newPath) );

    print_dir(curPath);


    string newFilePath = newPath + "/file1.txt";
    std::ofstream(newFilePath.c_str());
    tprintf(_T("Create file. (%s)\n"), newFilePath.c_str());

    newPath = curPath + "/tmp";
    tprintf(_T("DeleteDir(%s, ret : %d)\n"), newPath.c_str(), FileSystem::DeleteDir(newPath) );
    assert(!isDirExist(newPath));

    print_dir(curPath);

    FileSystem::DeleteDir(curPath + _T("/temp0"));
    bool bret = FileSystem::CreateDir(curPath + _T("/temp0/temp1/temp2/temp3/temp4"));
    assert(bret);

    assert(isDirExist(curPath + _T("/temp0")));
    assert(isDirExist(curPath + _T("/temp0/temp1")));
    assert(isDirExist(curPath + _T("/temp0/temp1/temp2")));
    assert(isDirExist(curPath + _T("/temp0/temp1/temp2/temp3")));
    assert(isDirExist(curPath + _T("/temp0/temp1/temp2/temp3/temp4")));

    // 이미 존재하는 경우 true를 리턴하는가?
    //bret = FileSystem::CreateDir(curPath + "/temp0/temp1/temp2/temp3/temp4");
    //assert(bret);

    FileSystem::DeleteDir(curPath + _T("/temp0"));
    assert(!isDirExist(curPath + _T("/temp0")));
}
