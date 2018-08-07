#pragma once

#include <string>
#include <algorithm>

#include <base/StringHelper.h>
#include <unistd.h> // getpid()

#include <experimental/filesystem>

#ifndef MAX_PATH
#define MAX_PATH    (1024)
#endif // MAX_PATH

using namespace grid;
using namespace grid::util;

namespace fs = std::experimental::filesystem;

namespace grid {

    namespace util {

        class FileSystem::_Impl {

        public:
            static tstring GetCurrentPath() {

                std::error_code ec;
                fs::path p = fs::current_path(ec);
                tstring strPath = p;

                return strPath;
            }

            static tstring GetFileName() {

                tchar szPath[MAX_PATH] = { 0 };
                tchar szTmp[MAX_PATH] = {0};

                sprintf(szTmp, "/proc/%d/exe", getpid());
                std::min((int)readlink(szTmp, szPath, MAX_PATH), MAX_PATH - 1);

                return fs::path(szPath).filename();
            }

            static uint32_t GetCurrentProcessId() {

                return getpid();
            }

            static bool CreateDir(const tstring& strDir) {

                if (strDir.empty()) {

                    return false;
                }

                fs::path p = strDir;
                std::error_code ec;
                return fs::create_directories(p, ec);
            }

            static bool DeleteDir(const tstring& strDir) {

                fs::path p = strDir;
                std::error_code ec;

                return fs::remove_all(p, ec);
            }

            static void RemoveFile(const tstring& strFilePath) {

                std::error_code ec;
                fs::path p = strFilePath;

                fs::remove(p,ec);
            }

            static uint32_t GetParentProcessId() {

                return getppid();
            }

            static tstring GetProcessPath(uint32_t nPid) {

                tstring strPath;
                tchar szTmp[MAX_PATH] = {0};
                tchar szPath[MAX_PATH] = { 0 };

                sprintf(szTmp, "/proc/%d/exe", nPid);

                std::min((int)readlink(szTmp, szPath, MAX_PATH), MAX_PATH - 1);

                strPath = szPath;

                tstring::size_type pos = strPath.rfind(_T('/'));

                if (pos == tstring::npos) {

                    return strPath;
                }

                strPath = strPath.substr(0, pos);

                return strPath;
            }

            static std::string GetSystemLocaleName() {

                return std::string("");
            }
        };
    }
}



