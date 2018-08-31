#include "TestBuffer.h"
#include "base/Buffer.h"

#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>
#include <cassert>


using namespace grid;


static Buffer MakeBuffer(std::size_t n) {
    //return BufferFactory::MakeDefaultBuffer(n);
    //return BufferFactory::MakePoolBuffer(n);
    return BufferFactory::MakeFixedSizePoolBuffer(n);
}

void TestBuffer::DoTest() {
    std::cout << "============= start Buffer test ===============" << std::endl;
    
	const uint32_t cap = 23;
	const std::string data = "0123456789";

	auto buffer = MakeBuffer(cap);
	assert(buffer.Capacity() == cap);

    //empty buffer
    {
        auto buffer = MakeBuffer(0);
        assert(buffer.Capacity() == 0);
        assert(buffer.Size() == 0);
        assert(buffer.Write(data.c_str(), data.length()) == 0);
        
        std::cout << "= empty buffer pass" << std::endl;
    }
    
    //AdjustToSize
    {
        auto buffer = MakeBuffer(cap);
        
        assert(buffer.Write(data.c_str(), data.length()) == data.length());
        
        const uint32_t skipBytes = 3;
        const uint32_t sizeBytes = 4;

        buffer.SkipPosToRead(skipBytes);
        buffer.AdjustToSize(sizeBytes);
        
        assert(buffer.Size() == sizeBytes);
        assert(std::memcmp(buffer.PosToRead(), data.substr(skipBytes).c_str(), sizeBytes) == 0);
        
        std::cout << "= AdjustToSize pass" << std::endl;
    }

    //write manually
    //assert(data.length() == buffer.Write(data.c_str(), data.length()));
    std::memcpy(buffer.PosToWrite(), data.c_str(), data.length());
    assert(buffer.Written(data.length()) == data.length());
    
    assert(buffer.Size() == data.length());
    assert(std::memcmp(buffer.PosToRead(), (data + data).c_str(), data.length()) == 0);
    assert(buffer.Available() == cap - data.length());
    
    std::cout << "= manual write pass" << std::endl;
    
    //write via method when sufficient memory
    assert(data.length() == buffer.Write(data.c_str(), data.length()));
    
    assert(buffer.Size() == data.length()*2);
    assert(std::memcmp(buffer.PosToRead(), (data + data).c_str(), data.length()*2) == 0);
    assert(buffer.Available() == cap - data.length()*2);

    std::cout << "= write method pass" << std::endl;
    
    //write via method when not sufficient memory
    const auto resultData = data + data + data.substr(0, cap % data.length());
    
    assert(cap % data.length() == buffer.Write(data.c_str(), data.length()));
    
    assert(buffer.Size() == cap);
    assert(std::memcmp(buffer.PosToRead(), resultData.c_str(), resultData.length()) == 0);
    assert(buffer.Available() == 0);

    std::cout << "= write() when not sufficient memory pass" << std::endl;
    
    //skip
    const uint32_t skipBytes = 3;

        //SkipPosToRead +
    {
        const auto skippedData = resultData.substr(skipBytes);
    
        buffer.SkipPosToRead(skipBytes);
        assert(buffer.Size() == skippedData.length());
        assert(std::memcmp(buffer.PosToRead(), skippedData.c_str(), skippedData.length()) == 0);

        std::cout << "= SkipPosToRead(+) pass" << std::endl;
    }
    
        //SkipPosToRead -
    {
        buffer.SkipPosToRead(-skipBytes);
        assert(buffer.Size() == resultData.length());
        assert(std::memcmp(buffer.PosToRead(), resultData.c_str(), resultData.length()) == 0);

        std::cout << "= SkipPosToRead(-) pass" << std::endl;
    }
    
        //SkipPosToWrite -
    {
        const auto skippedData = resultData.substr(0, resultData.length() - 3);
        
        buffer.SkipPosToWrite(-skipBytes);
        assert(buffer.Size() == skippedData.length());
        assert(std::memcmp(buffer.PosToRead(), skippedData.c_str(), skippedData.length()) == 0);

        std::cout << "= SkipPosToWrite(-) pass" << std::endl;
    }
    
        //SkipPosToWrite +
    {
        buffer.SkipPosToWrite(skipBytes);

        assert(buffer.Size() == resultData.length());
        assert(std::memcmp(buffer.PosToRead(), resultData.c_str(), resultData.length()) == 0);

        std::cout << "= SkipPosToWrite(+) pass" << std::endl;
    }
    
    //Duplicate 1
    auto bufferDup = buffer.Duplicate();
    assert(bufferDup.Memory() == buffer.Memory());
    assert(bufferDup.Size() == buffer.Size());
    assert(std::memcmp(bufferDup.PosToRead(), buffer.PosToRead(), buffer.Size()) == 0);

    std::cout << "= Duplicate 1 pass" << std::endl;

    //Duplicate 2
    {
        auto buffer = MakeBuffer(cap);
        
        assert(buffer.Write(data.c_str(), data.length()) == data.length());

        const uint32_t pos = 3;
        const uint32_t len = 4;
        
        auto bufferDup = buffer.Duplicate(pos, len);
        
        assert(bufferDup.Size() == len);
        assert(std::memcmp(bufferDup.PosToRead(), data.substr(pos, len).c_str(), len) == 0);

        std::cout << "= Duplicate 2 pass" << std::endl;
    }
    
    //Duplicate 3
    {
        auto buffer = MakeBuffer(cap);
        
        assert(buffer.Write(data.c_str(), data.length()) == data.length());
        
        const uint32_t pos = 3;
        const uint32_t len = data.length() - pos;
        
        auto bufferDup = buffer.Duplicate(pos, data.length());
        
        assert(bufferDup.Size() == len);
        assert(std::memcmp(bufferDup.PosToRead(), data.substr(pos, len).c_str(), len) == 0);
        
        std::cout << "= Duplicate 3 pass" << std::endl;
    }
    
    //copy ctor(shallow copy)
    auto bufferCtor = buffer;
    assert(bufferCtor.Memory() == buffer.Memory());
    assert(bufferCtor.Size() == buffer.Size());
    assert(std::memcmp(bufferCtor.PosToRead(), buffer.PosToRead(), buffer.Size()) == 0);

    std::cout << "= ctor() pass" << std::endl;
    
    //clone(deep copy)
    auto bufferClone = buffer.Clone();
    assert(bufferClone.Memory() != buffer.Memory());
    assert(bufferClone.Size() == buffer.Size());
    assert(std::memcmp(bufferClone.PosToRead(), buffer.PosToRead(), buffer.Size()) == 0);

    std::cout << "= Clone() pass" << std::endl;
    
    //WriteAtPos()
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer = MakeBuffer(cap);
        assert(buffer.Capacity() == cap);
        
        assert(data.length() == buffer.Write(data.c_str(), data.length()));
        assert(std::memcmp(buffer.PosToRead(), data.c_str(), data.length()) == 0);
        assert(buffer.Size() == data.length());
        assert(buffer.Available() == cap - data.length());

        // pos is over size
        const uint32_t gap = 4;
        uint32_t pos = data.length() + gap;
        assert(data.length() == buffer.WriteAtPos(data.c_str(), data.length(), pos));
        assert(buffer.Size() == pos + data.length());
        assert(std::memcmp(buffer.PosToRead(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.PosToRead()+pos, data.c_str(), data.length()) == 0);

        //pos is less size & post size is less size
        const std::string zero = "0000000000";
        const std::string zeroAndTail = zero + data.substr(data.length() - gap);
        pos = data.length();
        const auto prevSize = buffer.Size();
        assert(data.length() == buffer.WriteAtPos(zero.c_str(), zero.length(), pos));
        assert(prevSize == buffer.Size());
        assert(std::memcmp(buffer.PosToRead(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.PosToRead()+data.length(), zeroAndTail.c_str(), zeroAndTail.length()) == 0);

        //pos is less size & post size is over size
        const std::string zero2 = zero + zero;
        pos = data.length() * 2;
        assert(data.length() == buffer.WriteAtPos(zero.c_str(), zero.length(), pos));
        assert(pos + zero.length() == buffer.Size());
        assert(std::memcmp(buffer.PosToRead(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.PosToRead()+data.length(), zero2.c_str(), zero2.length()) == 0);

        std::cout << "= WriteAtPos() pass" << std::endl;
    }
    
    //WriteFromBuffer 1
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer1 = MakeBuffer(cap);
        assert(data.length() == buffer1.Write(data.c_str(), data.length()));

        const uint32_t skipBytes = 3;
        const uint32_t len = 4;
        
        auto buffer2 = MakeBuffer(cap);
        assert(len == buffer2.WriteFromBuffer(buffer1, skipBytes, len));
        assert(buffer2.Size() == len);
        assert(std::memcmp(buffer2.PosToRead(), data.substr(skipBytes, len).c_str(), len) == 0);
        
        std::cout << "= WriteFromBuffer 1 pass" << std::endl;
    }
    
    //WriteFromBuffer 2
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer1 = MakeBuffer(cap);
        assert(data.length() == buffer1.Write(data.c_str(), data.length()));
        
        const uint32_t skipBytes = 3;
        const uint32_t len = data.length() - skipBytes;
        
        auto buffer2 = MakeBuffer(cap);
        assert(len == buffer2.WriteFromBuffer(buffer1, skipBytes, data.length()));
        assert(buffer2.Size() == len);
        assert(std::memcmp(buffer2.PosToRead(), data.substr(skipBytes, len).c_str(), len) == 0);
        
        std::cout << "= WriteFromBuffer 2 pass" << std::endl;
    }
    
    std::cout << "============= complete Buffer test ===============" << std::endl;
}
