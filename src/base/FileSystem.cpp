#include <base/FileSystem.h>

#if defined(WIN32) || defined(_WIN64)
	#include <base/FileSystemWinImpl.hpp>
#elif defined(__APPLE__)
	#include <base/FileSystemMacImpl.hpp>
#else
	#include <base/FileSystemLinuxImpl.hpp>
#endif

using namespace grid;
using namespace grid::util;

tstring FileSystem::GetCurrentPath() {

    return _Impl::GetCurrentPath();
}

tstring FileSystem::GetFileName() {

    return _Impl::GetFileName();
}

uint32_t FileSystem::GetCurrentProcessId() {

    return _Impl::GetCurrentProcessId();
}

bool FileSystem::CreateDir(const tstring& strDir) {

    return _Impl::CreateDir(strDir);
}

bool FileSystem::DeleteDir(const tstring& strDir) {

    return _Impl::DeleteDir(strDir);
}

void FileSystem::RemoveFile(const tstring& strFilePath) {

    _Impl::RemoveFile(strFilePath);
}

uint32_t FileSystem::GetParentProcessId() {

    return _Impl::GetParentProcessId();
}

tstring FileSystem::GetProcessPath(uint32_t nPid) {

    return _Impl::GetProcessPath(nPid);
}

std::string FileSystem::GetSystemLocaleName() {

    return _Impl::GetSystemLocaleName();
}
