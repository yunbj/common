#include "base/Buffer.h"
#include "base/MemoryPool.h"
#include "base/ConcurrentFixedSizeMemoryPool.h"
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <cstring>


using namespace grid;


class DefaultBufferData : public IBufferData {
private:
    const std::size_t _capacity;
    uint8_t* _data;

public:
    explicit DefaultBufferData(uint32_t n)
        : _capacity(n)
        , _data(nullptr) {
        _data = new uint8_t[n];
    }

    ~DefaultBufferData() {
        if (_data) {
            delete[] _data;
            _data = nullptr;
        }
    }

    virtual uint32_t Capacity() const override { return _capacity; }
    
    virtual const void* Data() const override { return _data; }
    
    virtual void* Data() override { return _data; }
    
    virtual std::shared_ptr<IBufferData> Make(uint32_t size) override { return std::make_shared<DefaultBufferData>(size); }
};


/**
 */
class PoolBufferData : public IBufferData {
private:
    const std::size_t _capacity;
    uint8_t* _data;

public:
    explicit PoolBufferData(uint32_t n)
        : _capacity(n)
        , _data(nullptr) {
        _data = static_cast<uint8_t*>(MemoryPool::Instance().Alloc(n));
    }
    
    ~PoolBufferData() {
        if (_data) {
            MemoryPool::Instance().Dealloc(_data);
            _data = nullptr;
        }
    }

    virtual uint32_t Capacity() const override { return _capacity; }
    
    virtual const void* Data() const override { return _data; }
    
    virtual void* Data() override { return _data; }

    virtual std::shared_ptr<IBufferData> Make(uint32_t size) override { return std::make_shared<PoolBufferData>(size); }
};//class PoolBufferData


/**
 */
class FixedSizePoolBufferData : public IBufferData {
private:
    const std::size_t _capacity;
    uint8_t* _data;
    
public:
    explicit FixedSizePoolBufferData(uint32_t n)
        : _capacity(n)
        , _data(nullptr) {
        _data = static_cast<uint8_t*>(ConcurrentFixedSizeMemoryPool::Instance().Alloc(n));
    }
    
    ~FixedSizePoolBufferData() {
        if (_data) {
            ConcurrentFixedSizeMemoryPool::Instance().Dealloc(_data);
            _data = nullptr;
        }
    }
    
    virtual uint32_t Capacity() const override { return _capacity; }
    
    virtual const void* Data() const override { return _data; }
    
    virtual void* Data() override { return _data; }
    
    virtual std::shared_ptr<IBufferData> Make(uint32_t size) override { return std::make_shared<FixedSizePoolBufferData>(size); }
};//class FixedSizePoolBufferData



Buffer::Buffer(std::shared_ptr<IBufferData> data, uint32_t beginPos, uint32_t endPos)
    : _data(data)
    , _beginPos(beginPos)
    , _endPos(endPos) {
    assert(beginPos <= endPos);
}

Buffer::Buffer(std::shared_ptr<IBufferData> data, const void* ptr, uint32_t size)
    : _data(data)
    , _beginPos(0)
    , _endPos(0) {
    this->Write(ptr, size);
}

Buffer::Buffer(const Buffer& rhs)
    : Buffer(rhs._data, rhs._beginPos, rhs._endPos) {
}

Buffer& Buffer::operator=(const Buffer& rhs) {
    if (this != &rhs) {
        _data = rhs._data;
        _beginPos = rhs._beginPos;
        _endPos = rhs._endPos;
    }
    return *this;
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

uint32_t Buffer::Write(const void* ptr, uint32_t n) {
    auto bytes = std::min(n, this->Available());
    std::memcpy(this->PosToWrite(), ptr, bytes);
    _endPos += bytes;
    return bytes;
}

uint32_t Buffer::WriteAtPos(const void* ptr, uint32_t n, uint32_t pos) {
    if (pos >= _endPos) {
        this->SkipPosToWrite(pos - _endPos);
        return this->Write(ptr, n);
    } else {
        const auto oldEndPos = _endPos;
        this->SkipPosToWrite(pos - _endPos);
        const auto writtenBytes = this->Write(ptr, n);
        
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
    } else {
        return Buffer(_data->Make(0));
    }
}

Buffer Buffer::Clone() const {
    return Buffer(_data->Make(this->Size()), this->PosToRead(), this->Size());
}

bool Buffer::operator==(const Buffer& rhs) const {
    if (this->Size() == rhs.Size() && std::memcmp(this->Data(), rhs.Data(), this->Size()) == 0) {
        return true;
    }
    return false;
}

Buffer BufferFactory::MakeDefaultBuffer(uint32_t n) {
    return Buffer(std::make_shared<DefaultBufferData>(n));
}

Buffer BufferFactory::MakePoolBuffer(uint32_t n) {
    return Buffer(std::make_shared<PoolBufferData>(n));
}

Buffer BufferFactory::MakeFixedSizePoolBuffer(uint32_t n) {
    return Buffer(std::make_shared<FixedSizePoolBufferData>(n));
}

Buffer BufferFactory::MakeDefaultBuffer(const void* ptr, uint32_t n) {
    Buffer buf = MakeDefaultBuffer(n);
    buf.Write(ptr, n);
    return buf;
}

Buffer BufferFactory::MakePoolBuffer(const void* ptr, uint32_t n) {
    Buffer buf = MakePoolBuffer(n);
    buf.Write(ptr, n);
    return buf;
}

Buffer BufferFactory::MakeFixedSizePoolBuffer(const void* ptr, uint32_t n) {
    Buffer buf = MakeFixedSizePoolBuffer(n);
    buf.Write(ptr, n);
    return buf;
}

Buffer BufferFactory::MakeDefaultBuffer(const std::string& str) {
    return MakeDefaultBuffer(str.c_str(), str.length());
}

Buffer BufferFactory::MakePoolBuffer(const std::string& str) {
    return MakePoolBuffer(str.c_str(), str.length());
}

Buffer BufferFactory::MakeFixedSizePoolBuffer(const std::string& str) {
    return MakeFixedSizePoolBuffer(str.c_str(), str.length());
}
