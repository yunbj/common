#pragma once


#include <cstddef>


namespace grid
{


    /**
     *	@class	Pipe
     *	@brief	pipe object의 wrapper class
     */
    class Pipe {
    public://noncopyable
        Pipe(const Pipe&) = delete;
        Pipe(Pipe&&) = delete;
        Pipe& operator=(const Pipe&) = delete;
        Pipe& operator=(Pipe&&) = delete;
        
    public:
        Pipe();

        ~Pipe();

        int Open();

        int Close();

        bool IsOpen() const;

        int SetReadHandleToNonBlock();

        int SetWriteHandleToNonBlock();

        int GetReadHandle() const { return _handle[0]; }

        int GetWriteHandle() const { return _handle[1]; }

        int Send(const char* szBuf, std::size_t nBufLen);

        int Recv(char* szBuf, std::size_t nBufLen);

    private:
        int _handle[2];
    };// class Pipe


}// namespace grid
