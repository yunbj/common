#pragma once

#include <string>
#include <algorithm>

#include <base/StringHelper.h>
#include <unistd.h> // getpid()
#include <libproc.h>
#include <ftw.h>

#ifndef MAX_PATH
#define MAX_PATH    (1024)
#endif // MAX_PATH

namespace grid {

    namespace util {

        class FileSystem::_Impl {

        public:
            static tstring GetCurrentPath() {

                tstring str = GetProcessPath(GetCurrentProcessId());
                util::StringHelper::RTrim(str, DIR_DELIMETER);
                
                return str;
            }

            static tstring GetFileName() {

                tchar szPath[MAX_PATH] = { 0 };
                tstring strPath;

                int ret = proc_name(getpid(), szPath, sizeof(szPath));

                if(ret <= 0) {

                    return tstring();
                }

                strPath = szPath;

                return strPath;
            }

            static uint32_t GetCurrentProcessId() {

                return getpid();
            }

            static bool isDirExist(const std::string& path) {

                struct stat info;
                if (stat(path.c_str(), &info) != 0) {

                    return false;
                }
                return (info.st_mode & S_IFDIR) != 0;
            }
            
            static bool CreateDir(const tstring& path) {

                if (path.empty()) {

                    return true;
                }
                
                mode_t mode = 0755;
                int ret = mkdir(path.c_str(), mode);
                
                if (ret == 0) {

                    return true;
                }
                
                switch (errno) {

                    case ENOENT: {

                        // parent didn't exist, try to create it
                        std::string::size_type pos = path.find_last_of('/');
                        if (pos == std::string::npos) {

                            return false;
                        }

                        if (!CreateDir( path.substr(0, pos) )) {

                            return false;
                        }
                    }
                        // now, try to create again
                        return 0 == mkdir(path.c_str(), mode);
                        
                    case EEXIST:
                        // done!
                        return isDirExist(path);
                        
                    default:
                        return false;
                }
                
                return true;
            }
            
            static bool DeleteDir(const tstring& path) {

                if (path.empty()) {

                    return false;
                }
                
                if(!isDirExist(path)) {

                    return true;
                }
                
                int flags = 0;
                int nopenfs = 10;
                auto rmdirHelper = [](const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){

                    switch ( tflag ) {
                        case FTW_D:
                        case FTW_DP:
                            rmdir(fpath);
                            break;
                            
                        case FTW_F:
                        case FTW_SL:
                            unlink(fpath);
                            break;
                            
                        default:
                            break;
                    }
                    return 0;
                };
                
                flags |= FTW_DEPTH; // post-order traverse
                if (nftw(path.c_str(), rmdirHelper, nopenfs, flags) == -1) {

                    return false;
                }
                
                return true;
            }

            static void RemoveFile(const tstring& strFilePath) {

                std::remove(strFilePath.c_str());
            }


            static uint32_t GetParentProcessId() {

                return getppid();
            }

            static tstring GetProcessPath(uint32_t nPid) {

                tchar szPath[MAX_PATH] = { 0 };
                tstring strPath;

                int ret = proc_pidpath(nPid, szPath, sizeof(szPath));

                if(ret <= 0) {

                    return tstring();
                }

                strPath = szPath;
                tstring::size_type pos = strPath.rfind(DIR_DELIMETER);
                
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
