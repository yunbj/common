#pragma once


#include <cstddef>


namespace grid
{


/**
 *	@class	Pipe
 *	@brief	pipe object의 wrapper class
 */
class Pipe
{
public://noncopyable
    Pipe(const Pipe&) = delete;
    Pipe(Pipe&&) = delete;
    Pipe& operator=(const Pipe&) = delete;
    Pipe& operator=(Pipe&&) = delete;
    
public:
	Pipe();

	~Pipe();

	int open();

	int close();

	bool isOpen() const;

	int setReadHandleToNonBlock();

	int setWriteHandleToNonBlock();

	int getReadHandle() const { return _handle[0]; }

	int getWriteHandle() const { return _handle[1]; }

    int send(const char* szBuf, std::size_t nBufLen);

	int recv(char* szBuf, std::size_t nBufLen);

private:
	int _handle[2];
};// class Pipe


}// namespace grid
