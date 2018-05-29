#pragma once

#include <base/FileSystem.h>
#include <Windows.h>
#include <tlhelp32.h>

#include <string>
#include <algorithm>

#include <base/StringHelper.h>
#include <base/StringConvertor.h>
#include <base/LogMgr.h>

namespace grid
{
	namespace util
	{
        class FileSystem::_Impl
        {
            static tstring GetCurrentPath()
            {
                fs::path p = fs::current_path();
                tstring strPath = p;

                return strPath;
            }

            static tstring GetFileName()
            {
                tstring strPath;
                tchar szPath[MAX_PATH] = { 0 };

                GetModuleFileName(nullptr, szPath, MAX_PATH - 1);

                strPath = szPath;
                StringHelper::ToLower(strPath);

                tstring::size_type pos = strPath.rfind(_T('\\'));

                if (pos == tstring::npos)
                {
                    return strPath;
                }

                strPath = strPath.substr(pos + 1);

                return strPath;
            }

            static uint32_t GetCurrentProcessId()
            {
                return ::GetCurrentProcessId();
            }

            static bool CreateDir(const tstring& strDir)
            {
                if (strDir.empty())
                {
                    return false;
                }

                fs::path p = strDir;
                return fs::create_directories(p);
            }

            static bool DeleteDir(const tstring& strDir)
            {
                fs::path p = strDir;

                return fs::remove_all(p);
            }

            static void RemoveFile(const tstring& strFilePath)
            {
                fs::path p = strFilePath;
                fs::remove(p);
            }

            static uint32_t GetParentProcessId()
            {
                PROCESSENTRY32 processInfo;
                processInfo.dwSize = sizeof(processInfo);

                HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, static_cast<DWORD>(0));
                if (processesSnapshot == INVALID_HANDLE_VALUE)
                {
                    return 0;
                }

                Process32First(processesSnapshot, &processInfo);
                DWORD nCurProcessId = GetCurrentProcessId();

                if (nCurProcessId == processInfo.th32ProcessID)
                {
                    CloseHandle(processesSnapshot);
                    return processInfo.th32ParentProcessID;
                }

                while (Process32Next(processesSnapshot, &processInfo))
                {
                    if (nCurProcessId == processInfo.th32ProcessID)
                    {
                        CloseHandle(processesSnapshot);
                        return processInfo.th32ParentProcessID;
                    }
                }

                CloseHandle(processesSnapshot);
                return 0;
            }

            static tstring GetProcessPath(uint32_t nPid)
            {
                HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, nPid);

                if (hProcess == nullptr)
                {
                    ERROR_LOG(_T("[filesys] failed open process.(pid:%u, err:%d)"), nPid, GetLastError());
                    return _T("");
                }

                tchar szPath[MAX_PATH] = { 0 };
                DWORD dwLen = sizeof(szPath) - 1;

                if (QueryFullProcessImageName(hProcess, 0, szPath, &dwLen))
                {
                    tstring strPath(szPath, dwLen);

                    StringHelper::ToLower(strPath);

                    return strPath;
                }

                ERROR_LOG(_T("[filesys] failed query process path.(pid:%u, err:%d)"), nPid, GetLastError());

                return _T("");
            }

            static std::string GetSystemLocaleName()
            {
                wchar_t szLocaleInfo[MAX_PATH];
                GetSystemDefaultLocaleName(szLocaleInfo, MAX_PATH);

                return GRID_W2A(szLocaleInfo);
            }
        };
	} // end namespace util
} // end namesapce grid
