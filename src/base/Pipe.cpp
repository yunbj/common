#include <base/Pipe.h>
#include <unistd.h>
#include <fcntl.h>


using namespace grid;


Pipe::Pipe() {
	_handle[0] = _handle[1] = -1;
}

Pipe::~Pipe() {
	this->Close();
}

int Pipe::Open() {
	if (::pipe(_handle) == -1) {
		return -1;
	}

	::fcntl(this->GetReadHandle(), F_SETFD, FD_CLOEXEC);
	::fcntl(this->GetWriteHandle(), F_SETFD, FD_CLOEXEC);

	return 0;
}

int Pipe::Close() {
	int nRet = 0;

	if (_handle[0] != -1) {
		nRet = ::close(_handle[0]);
		_handle[0] = -1;
	}

	if (_handle[1] != -1) {
		nRet |= ::close(_handle[1]);
		_handle[1] = -1;
	}

	return nRet;
}

bool Pipe::IsOpen() const {
	return _handle[0] != -1 && _handle[1] != -1;
}

int Pipe::SetReadHandleToNonBlock() {
	int nOldFlag = ::fcntl(this->GetReadHandle(), F_GETFL);
	return ::fcntl(this->GetReadHandle(), F_SETFL, nOldFlag | O_NONBLOCK);
}

int Pipe::SetWriteHandleToNonBlock() {
	int nOldFlag = ::fcntl(this->GetWriteHandle(), F_GETFL);
	return ::fcntl(this->GetWriteHandle(), F_SETFL, nOldFlag | O_NONBLOCK);
}

int Pipe::Send(const char* szBuf, std::size_t nBufLen) {
	if (this->GetWriteHandle() == -1) {
		return -1;
	}

	if (szBuf == 0 || nBufLen == 0) {
		return -1;
	}

	return ::write(this->GetWriteHandle(), szBuf, nBufLen);
}

int Pipe::Recv(char* szBuf, std::size_t nBufLen) {
	if (this->GetReadHandle() == -1) {
		return -1;
	}

	if (szBuf == 0 || nBufLen == 0) {
		return -1;
	}

	return ::read(this->GetReadHandle(), szBuf, nBufLen);
}
