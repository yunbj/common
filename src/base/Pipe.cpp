#include <base/Pipe.h>
#include <unistd.h>
#include <fcntl.h>


using namespace grid;


Pipe::Pipe()
{
	_handle[0] = _handle[1] = -1;
}

Pipe::~Pipe()
{
	this->close();
}

int Pipe::open()
{
	if (::pipe(_handle) == -1)
	{
		return -1;
	}

	::fcntl(this->getReadHandle(), F_SETFD, FD_CLOEXEC);
	::fcntl(this->getWriteHandle(), F_SETFD, FD_CLOEXEC);

	return 0;
}

int Pipe::close()
{
	int nRet = 0;

	if (_handle[0] != -1)
	{
		nRet = ::close(_handle[0]);
		_handle[0] = -1;
	}

	if (_handle[1] != -1)
	{
		nRet |= ::close(_handle[1]);
		_handle[1] = -1;
	}

	return nRet;
}

bool Pipe::isOpen() const
{
	return _handle[0] != -1 && _handle[1] != -1;
}


int Pipe::setReadHandleToNonBlock()
{
	int nOldFlag = ::fcntl(this->getReadHandle(), F_GETFL);
	return ::fcntl(this->getReadHandle(), F_SETFL, nOldFlag | O_NONBLOCK);
}

int Pipe::setWriteHandleToNonBlock()
{
	int nOldFlag = ::fcntl(this->getWriteHandle(), F_GETFL);
	return ::fcntl(this->getWriteHandle(), F_SETFL, nOldFlag | O_NONBLOCK);
}

int Pipe::send(const char* szBuf, std::size_t nBufLen)
{
	if (this->getWriteHandle() == -1)
	{
		return -1;
	}

	if (szBuf == 0 || nBufLen == 0)
	{
		return -1;
	}

	return ::write(this->getWriteHandle(), szBuf, nBufLen);
}

int Pipe::recv(char* szBuf, std::size_t nBufLen)
{
	if (this->getReadHandle() == -1)
	{
		return -1;
	}

	if (szBuf == 0 || nBufLen == 0)
	{
		return -1;
	}

	return ::read(this->getReadHandle(), szBuf, nBufLen);
}
