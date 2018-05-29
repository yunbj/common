#pragma once

#include <base/Base.h>

namespace grid
{
	namespace util
	{
		class FileSystem
		{
		public:
			static tstring GetCurrentPath();// get current path
			static tstring GetFileName();	// get grid client binary name

			static bool CreateDir(const tstring& strDir);
			static bool DeleteDir(const tstring& strDir);
			static void RemoveFile(const tstring& strFilePath);

			static std::string GetSystemLocaleName();


            static uint32_t GetCurrentProcessId();
			/*
				return if success parent process id. otherwise 0.
			*/
			static uint32_t GetParentProcessId();
			/*
				return if success nPid's full path. otherwise empty string.
			*/
			static tstring GetProcessPath(uint32_t nPid);

        private:
            class _Impl;
		};
	}
}
