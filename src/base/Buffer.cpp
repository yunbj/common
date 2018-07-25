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
        if (n > 0)
        {
            _data = new uint8_t[n];
        }
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
    

Buffer::Buffer(std::shared_ptr<IBufferData> data, uint32_t beginPos, uint32_t endPos)
    : _data(data)
    , _beginPos(beginPos)
    , _endPos(endPos)
{
    assert(beginPos <= endPos);
}

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
    return this->posToRead();
}

const void* Buffer::memory() const
{
    return const_cast<Buffer*>(this)->data();
}

void* Buffer::memory()
{
    return _data->data();
}

const void* Buffer::posToRead() const
{
    return const_cast<Buffer*>(this)->posToRead();
}

void* Buffer::posToRead()
{
    return (uint8_t*)_data->data() + _beginPos;
}

const void* Buffer::posToWrite() const
{
    return const_cast<Buffer*>(this)->posToWrite();
}

void* Buffer::posToWrite()
{
    return (uint8_t*)_data->data() + _endPos;
}

uint32_t Buffer::available() const
{
    return _data->capacity() - _endPos;
}

void Buffer::skipPosToRead(int32_t n)
{
    const uint32_t pos = _beginPos + n;
    if (pos <= _endPos)
    {
        _beginPos = pos;
    }
}

void Buffer::skipPosToWrite(int32_t n)
{
    auto pos = _endPos + n;
    if (_beginPos <= pos && pos <= _data->capacity())
    {
        _endPos = pos;
    }
}

void Buffer::adjustToSize(uint32_t n)
{
    const auto pos = _beginPos + n;
    if (pos <= _data->capacity())
    {
        _endPos = pos;
    }
}

uint32_t Buffer::written(uint32_t n)
{
    auto bytes = std::min(n, this->available());
    _endPos += bytes;
    return bytes;
}

uint32_t Buffer::write(const void* data, uint32_t n)
{
    auto bytes = std::min(n, this->available());
    std::memcpy(this->posToWrite(), data, bytes);
    _endPos += bytes;
    return bytes;
}

uint32_t Buffer::writeAtPos(const void* data, uint32_t n, uint32_t pos)
{
    if (pos >= _endPos)
    {
        this->skipPosToWrite(pos - _endPos);
        return this->write(data, n);
    }
    else
    {
        const auto oldEndPos = _endPos;
        this->skipPosToWrite(pos - _endPos);
        const auto writtenBytes = this->write(data, n);
        
        if (_endPos < oldEndPos)
        {
            _endPos = oldEndPos;
        }
        
        return writtenBytes;
    }
}

uint32_t Buffer::writeFromBuffer(const Buffer& buffer, uint32_t pos, uint32_t n)
{
    auto dup = buffer.duplicate(pos, n);
    return this->write(dup.posToRead(), dup.size());
}

Buffer Buffer::duplicate() const
{
    return *this;
}

Buffer Buffer::duplicate(uint32_t pos, uint32_t n) const
{
    if (_beginPos <= pos)
    {
        const auto endPos = std::min(pos + n, _endPos);
        
        return Buffer(_data, pos, endPos);
    }
    else
    {
        return Buffer(0, Buffer::MemoryPolicy::Default);
    }
}

Buffer Buffer::clone() const
{
    return Buffer(this->posToRead(), this->size(), Buffer::MemoryPolicy::Default);
}

Buffer BufferFactory::makeDefaultBuffer(uint32_t n)
{
    return Buffer(n, Buffer::MemoryPolicy::Default);
}

Buffer BufferFactory::makePoolBuffer(uint32_t n)
{
    return Buffer(n, Buffer::MemoryPolicy::Pool);
}


