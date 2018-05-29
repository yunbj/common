//
// Created by yunbj on 17. 5. 24.
//

#include <string>
#include <fcntl.h>
#include <unistd.h>

#include <base/LogMgr.h>
#include <cstring>

#include <base/PidFile.h>



namespace grid::process {

    PidFile::PidFile()
    {
    }

    PidFile::~PidFile()
    {
    }

    ////////////////////////////////////////////////////

    static bool lockfile(int lockFd)
    {
        struct flock fl;

        fl.l_type = F_WRLCK;
        fl.l_start = 0;
        fl.l_whence = SEEK_SET;
        fl.l_len = 0;

        return (fcntl(lockFd, F_SETLK, &fl) == 0);
    }

    bool PidFile::CreatePidFile(const std::string& pidFileName)
    {
        _lockFd = open(pidFileName.c_str(), O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if( _lockFd < 0 )
        {
            ERROR_LOG("Cannot open %s : %s", pidFileName.c_str(), strerror(errno));
            return false;
        }

        if( !lockfile(_lockFd) )
        {
            if( errno == EACCES || errno == EAGAIN )
            {
                ERROR_LOG("Already started");
                close(_lockFd);
            } else {
                ERROR_LOG("Cannot lock %s: %s", pidFileName.c_str(), strerror(errno));
            }
            return false;
        }

        ftruncate(_lockFd, 0);
        char pidBuf[16];
        sprintf(pidBuf, "%ld", (long)getpid());
        write(_lockFd, pidBuf, strlen(pidBuf));

        _pidFileName = pidFileName;
        return true;
    }

    void PidFile::ClosePidFile()
    {
        if( _lockFd > 0 )
        {
            close(_lockFd);
            _lockFd = -1;
        }
    }

    void PidFile::RemovePidFile()
    {
        std::remove(_pidFileName.c_str());
    }

}

