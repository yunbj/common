//
// Created by yunbj on 17. 5. 24.
//

#ifndef STREAMINGSERVER_PPLOCKFILE_H
#define STREAMINGSERVER_PPLOCKFILE_H

namespace grid::process {
    class PidFile
    {
    public:
        PidFile();
        ~PidFile();

        bool CreatePidFile(const std::string& pidFileName);
        void ClosePidFile();
        void RemovePidFile();

    private:
        int _lockFd{-1};
        std::string _pidFileName{};
    };
}

#endif //STREAMINGSERVER_PPLOCKFILE_H
