#include "base/Buffer.h"


#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <cstring>


using namespace grid;


class DefaultBufferData : public IBufferData
{
private:
	uint8_t* _data;
	const std::size_t _capacity;

public:
	explicit DefaultBufferData(uint32_t n)
		: _data(nullptr)
		, _capacity(n)
	{
		assert(n > 0);

		_data = new uint8_t[n];
	}

	~DefaultBufferData()
	{
		if (_data)
		{
			delete[] _data;
			_data = nullptr;
		}
	}

	virtual uint32_t capacity() const
	{
		return _capacity;
	}

    virtual void* data()
    {
        return _data;
    }
    
    virtual const void* data() const
    {
        return _data;
    }
};//class DefaultBufferData
    

Buffer::Buffer(uint32_t n, MemoryPolicy memPolicy)
    : _data()
    , _beginPos(0)
    , _endPos(0)
{
    _data = std::make_shared<DefaultBufferData>(n);
}

Buffer::Buffer(const void* data, uint32_t size, MemoryPolicy memPolicy)
    : _data()
    , _beginPos(0)
    , _endPos(0)
{
    _data = std::make_shared<DefaultBufferData>(size);
    this->write(data, size);
}

uint32_t Buffer::capacity() const
{
    return _data->capacity();
}

uint32_t Buffer::size() const
{
    return _endPos - _beginPos;
}

const void* Buffer::data() const
{
    return const_cast<Buffer*>(this)->data();
}

void* Buffer::data()
{
    return (uint8_t*)_data->data() + _beginPos;
}

uint32_t Buffer::available() const
{
    return _data->capacity() - _endPos;
}

void Buffer::skipBytes(uint32_t n)
{
    auto pos = _beginPos + n;
    if (pos <= _endPos)
    {
        _beginPos = pos;
    }
}

void* Buffer::_getEndPos() const
{
    return (uint8_t*)_data->data() + _endPos;
}

uint32_t Buffer::write(const void* data, uint32_t n)
{
    auto bytes = std::min(n, this->available());
    std::memcpy(this->_getEndPos(), data, bytes);
    _endPos += bytes;
    return bytes;
}

Buffer Buffer::duplicate() const
{
    return *this;
}

Buffer Buffer::clone() const
{
    return Buffer(this->data(), this->size(), Buffer::MemoryPolicy::Default);
}

Buffer BufferFactory::makeDefaultBuffer(uint32_t n)
{
    return Buffer(n, Buffer::MemoryPolicy::Default);
}

Buffer BufferFactory::makePoolBuffer(uint32_t n)
{
    return Buffer(n, Buffer::MemoryPolicy::Pool);
}


