#include "base/Buffer.h"
#include "base/MemoryPool.h"

#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <cstring>


using namespace grid;


class PoolBufferData : public IBufferData {
private:
    uint8_t* _data;
    const std::size_t _capacity;
    
public:
    explicit PoolBufferData(uint32_t n)
        : _data(nullptr)
        , _capacity(n) {
        _data = static_cast<uint8_t*>(MemoryPool::Instance().Alloc(n));
    }
    
    ~PoolBufferData() {
        if (_data)
        {
            MemoryPool::Instance().Dealloc(_data);
            _data = nullptr;
        }
    }
    
    virtual uint32_t Capacity() const {
        return _capacity;
    }
    
    virtual void* Data() {
        return _data;
    }
    
    virtual const void* Data() const {
        return _data;
    }
    
    virtual MemoryPolicy Policy() const {
        return MemoryPolicy::Pool;
    }
};//class PoolBufferData


class DefaultBufferData : public IBufferData {
private:
	uint8_t* _data;
	const std::size_t _capacity;

public:
    explicit DefaultBufferData(uint32_t n)
		: _data(nullptr)
		, _capacity(n) {
        if (n > 0) {
            _data = new uint8_t[n];
        }
	}

	~DefaultBufferData() {
		if (_data) {
            delete[] _data;
			_data = nullptr;
		}
	}

	virtual uint32_t Capacity() const {
		return _capacity;
	}

    virtual void* Data() {
        return _data;
    }
    
    virtual const void* Data() const {
        return _data;
    }

    virtual MemoryPolicy Policy() const {
        return MemoryPolicy::Default;
    }
};//class DefaultBufferData
    

std::shared_ptr<IBufferData> IBufferData::Make(uint32_t n, MemoryPolicy policy) {
    if (policy == MemoryPolicy::Pool) {
        return std::make_shared<PoolBufferData>(n);
    }
    else {
        return std::make_shared<DefaultBufferData>(n);
    }
}


Buffer::Buffer(std::shared_ptr<IBufferData> data, uint32_t beginPos, uint32_t endPos)
    : _data(data)
    , _beginPos(beginPos)
    , _endPos(endPos) {
    assert(beginPos <= endPos);
}

Buffer::Buffer(uint32_t n, MemoryPolicy memPolicy)
    : _data()
    , _beginPos(0)
    , _endPos(0) {
    _data = IBufferData::Make(n, memPolicy);
}

Buffer::Buffer(const void* data, uint32_t size, MemoryPolicy memPolicy)
    : _data()
    , _beginPos(0)
    , _endPos(0) {
    _data = IBufferData::Make(size, memPolicy);

    this->Write(data, size);
}

uint32_t Buffer::Capacity() const {
    return _data->Capacity();
}

uint32_t Buffer::Size() const {
    return _endPos - _beginPos;
}

const void* Buffer::Data() const {
    return const_cast<Buffer*>(this)->Data();
}

void* Buffer::Data() {
    return this->PosToRead();
}

const void* Buffer::Memory() const {
    return const_cast<Buffer*>(this)->Data();
}

void* Buffer::Memory() {
    return _data->Data();
}

const void* Buffer::PosToRead() const {
    return const_cast<Buffer*>(this)->PosToRead();
}

void* Buffer::PosToRead() {
    return (uint8_t*)_data->Data() + _beginPos;
}

const void* Buffer::PosToWrite() const {
    return const_cast<Buffer*>(this)->PosToWrite();
}

void* Buffer::PosToWrite() {
    return (uint8_t*)_data->Data() + _endPos;
}

uint32_t Buffer::Available() const {
    return _data->Capacity() - _endPos;
}

void Buffer::SkipPosToRead(int32_t n) {
    const uint32_t pos = _beginPos + n;
    if (pos <= _endPos) {
        _beginPos = pos;
    }
}

void Buffer::SkipPosToWrite(int32_t n) {
    auto pos = _endPos + n;
    if (_beginPos <= pos && pos <= _data->Capacity()) {
        _endPos = pos;
    }
}

void Buffer::AdjustToSize(uint32_t n) {
    const auto pos = _beginPos + n;
    if (pos <= _data->Capacity()) {
        _endPos = pos;
    }
}

uint32_t Buffer::Written(uint32_t n) {
    auto bytes = std::min(n, this->Available());
    _endPos += bytes;
    return bytes;
}

uint32_t Buffer::Write(const void* data, uint32_t n) {
    auto bytes = std::min(n, this->Available());
    std::memcpy(this->PosToWrite(), data, bytes);
    _endPos += bytes;
    return bytes;
}

uint32_t Buffer::WriteAtPos(const void* data, uint32_t n, uint32_t pos) {
    if (pos >= _endPos) {
        this->SkipPosToWrite(pos - _endPos);
        return this->Write(data, n);
    }
    else {
        const auto oldEndPos = _endPos;
        this->SkipPosToWrite(pos - _endPos);
        const auto writtenBytes = this->Write(data, n);
        
        if (_endPos < oldEndPos) {
            _endPos = oldEndPos;
        }
        
        return writtenBytes;
    }
}

uint32_t Buffer::WriteFromBuffer(const Buffer& buffer, uint32_t pos, uint32_t n) {
    auto dup = buffer.Duplicate(pos, n);
    return this->Write(dup.PosToRead(), dup.Size());
}

Buffer Buffer::Duplicate() const {
    return *this;
}

Buffer Buffer::Duplicate(uint32_t pos, uint32_t n) const {
    if (_beginPos <= pos) {
        const auto endPos = std::min(pos + n, _endPos);
        
        return Buffer(_data, pos, endPos);
    }
    else {
        return Buffer(0, _data->Policy());
    }
}

Buffer Buffer::Clone() const {
    return Buffer(this->PosToRead(), this->Size(), _data->Policy());
}

Buffer BufferFactory::MakeDefaultBuffer(uint32_t n) {
    return Buffer(n, MemoryPolicy::Default);
    //return makePoolBuffer(n);
}

Buffer BufferFactory::MakePoolBuffer(uint32_t n) {
    return Buffer(n, MemoryPolicy::Pool);
}
