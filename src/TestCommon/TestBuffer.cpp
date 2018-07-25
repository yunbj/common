#include "TestBuffer.h"
#include "base/Buffer.h"

#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>
#include <cassert>


using namespace grid;


void TestBuffer::DoTest()
{
    std::cout << "============= start Buffer test ===============" << std::endl;
    
	const uint32_t cap = 23;
	const std::string data = "0123456789";

	auto buffer = BufferFactory::makeDefaultBuffer(cap);
	assert(buffer.capacity() == cap);

    //empty buffer
    {
        auto buffer = BufferFactory::makeDefaultBuffer(0);
        assert(buffer.capacity() == 0);
        assert(buffer.size() == 0);
        assert(buffer.write(data.c_str(), data.length()) == 0);
        
        std::cout << "= empty buffer pass" << std::endl;
    }
    
    //adjustToSize
    {
        auto buffer = BufferFactory::makeDefaultBuffer(cap);
        
        assert(buffer.write(data.c_str(), data.length()) == data.length());
        
        const uint32_t skipBytes = 3;
        const uint32_t sizeBytes = 4;

        buffer.skipPosToRead(skipBytes);
        buffer.adjustToSize(sizeBytes);
        
        assert(buffer.size() == sizeBytes);
        assert(std::memcmp(buffer.posToRead(), data.substr(skipBytes).c_str(), sizeBytes) == 0);
        
        std::cout << "= adjustToSize pass" << std::endl;
    }

    //write manually
    //assert(data.length() == buffer.write(data.c_str(), data.length()));
    std::memcpy(buffer.posToWrite(), data.c_str(), data.length());
    assert(buffer.written(data.length()) == data.length());
    
    assert(buffer.size() == data.length());
    assert(std::memcmp(buffer.posToRead(), (data + data).c_str(), data.length()) == 0);
    assert(buffer.available() == cap - data.length());
    
    std::cout << "= manual write pass" << std::endl;
    
    //write via method when sufficient memory
    assert(data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == data.length()*2);
    assert(std::memcmp(buffer.posToRead(), (data + data).c_str(), data.length()*2) == 0);
    assert(buffer.available() == cap - data.length()*2);

    std::cout << "= write method pass" << std::endl;
    
    //write via method when not sufficient memory
    const auto resultData = data + data + data.substr(0, cap % data.length());
    
    assert(cap % data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == cap);
    assert(std::memcmp(buffer.posToRead(), resultData.c_str(), resultData.length()) == 0);
    assert(buffer.available() == 0);

    std::cout << "= write() when not sufficient memory pass" << std::endl;
    
    //skip
    const uint32_t skipBytes = 3;

        //skipPosToRead +
    {
        const auto skippedData = resultData.substr(skipBytes);
    
        buffer.skipPosToRead(skipBytes);
        assert(buffer.size() == skippedData.length());
        assert(std::memcmp(buffer.posToRead(), skippedData.c_str(), skippedData.length()) == 0);

        std::cout << "= skipPosToRead(+) pass" << std::endl;
    }
    
        //skipPosToRead -
    {
        buffer.skipPosToRead(-skipBytes);
        assert(buffer.size() == resultData.length());
        assert(std::memcmp(buffer.posToRead(), resultData.c_str(), resultData.length()) == 0);

        std::cout << "= skipPosToRead(-) pass" << std::endl;
    }
    
        //skipPosToWrite -
    {
        const auto skippedData = resultData.substr(0, resultData.length() - 3);
        
        buffer.skipPosToWrite(-skipBytes);
        assert(buffer.size() == skippedData.length());
        assert(std::memcmp(buffer.posToRead(), skippedData.c_str(), skippedData.length()) == 0);

        std::cout << "= skipPosToWrite(-) pass" << std::endl;
    }
    
        //skipPosToWrite +
    {
        buffer.skipPosToWrite(skipBytes);

        assert(buffer.size() == resultData.length());
        assert(std::memcmp(buffer.posToRead(), resultData.c_str(), resultData.length()) == 0);

        std::cout << "= skipPosToWrite(+) pass" << std::endl;
    }
    
    //duplicate 1
    auto bufferDup = buffer.duplicate();
    assert(bufferDup.memory() == buffer.memory());
    assert(bufferDup.size() == buffer.size());
    assert(std::memcmp(bufferDup.posToRead(), buffer.posToRead(), buffer.size()) == 0);

    std::cout << "= duplicate 1 pass" << std::endl;

    //duplicate 2
    {
        auto buffer = BufferFactory::makeDefaultBuffer(cap);
        
        assert(buffer.write(data.c_str(), data.length()) == data.length());

        const uint32_t pos = 3;
        const uint32_t len = 4;
        
        auto bufferDup = buffer.duplicate(pos, len);
        
        assert(bufferDup.size() == len);
        assert(std::memcmp(bufferDup.posToRead(), data.substr(pos, len).c_str(), len) == 0);

        std::cout << "= duplicate 2 pass" << std::endl;
    }
    
    //duplicate 3
    {
        auto buffer = BufferFactory::makeDefaultBuffer(cap);
        
        assert(buffer.write(data.c_str(), data.length()) == data.length());
        
        const uint32_t pos = 3;
        const uint32_t len = data.length() - pos;
        
        auto bufferDup = buffer.duplicate(pos, data.length());
        
        assert(bufferDup.size() == len);
        assert(std::memcmp(bufferDup.posToRead(), data.substr(pos, len).c_str(), len) == 0);
        
        std::cout << "= duplicate 3 pass" << std::endl;
    }
    
    //copy ctor(shallow copy)
    auto bufferCtor = buffer;
    assert(bufferCtor.memory() == buffer.memory());
    assert(bufferCtor.size() == buffer.size());
    assert(std::memcmp(bufferCtor.posToRead(), buffer.posToRead(), buffer.size()) == 0);

    std::cout << "= ctor() pass" << std::endl;
    
    //clone(deep copy)
    auto bufferClone = buffer.clone();
    assert(bufferClone.memory() != buffer.memory());
    assert(bufferClone.size() == buffer.size());
    assert(std::memcmp(bufferClone.posToRead(), buffer.posToRead(), buffer.size()) == 0);

    std::cout << "= clone() pass" << std::endl;
    
    //writeAtPos()
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer = BufferFactory::makeDefaultBuffer(cap);
        assert(buffer.capacity() == cap);
        
        assert(data.length() == buffer.write(data.c_str(), data.length()));
        assert(std::memcmp(buffer.posToRead(), data.c_str(), data.length()) == 0);
        assert(buffer.size() == data.length());
        assert(buffer.available() == cap - data.length());

        // pos is over size
        const uint32_t gap = 4;
        uint32_t pos = data.length() + gap;
        assert(data.length() == buffer.writeAtPos(data.c_str(), data.length(), pos));
        assert(buffer.size() == pos + data.length());
        assert(std::memcmp(buffer.posToRead(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.posToRead()+pos, data.c_str(), data.length()) == 0);

        //pos is less size & post size is less size
        const std::string zero = "0000000000";
        const std::string zeroAndTail = zero + data.substr(data.length() - gap);
        pos = data.length();
        const auto prevSize = buffer.size();
        assert(data.length() == buffer.writeAtPos(zero.c_str(), zero.length(), pos));
        assert(prevSize == buffer.size());
        assert(std::memcmp(buffer.posToRead(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.posToRead()+data.length(), zeroAndTail.c_str(), zeroAndTail.length()) == 0);

        //pos is less size & post size is over size
        const std::string zero2 = zero + zero;
        pos = data.length() * 2;
        assert(data.length() == buffer.writeAtPos(zero.c_str(), zero.length(), pos));
        assert(pos + zero.length() == buffer.size());
        assert(std::memcmp(buffer.posToRead(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.posToRead()+data.length(), zero2.c_str(), zero2.length()) == 0);

        std::cout << "= writeAtPos() pass" << std::endl;
    }
    
    //writeFromBuffer 1
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer1 = BufferFactory::makeDefaultBuffer(cap);
        assert(data.length() == buffer1.write(data.c_str(), data.length()));

        const uint32_t skipBytes = 3;
        const uint32_t len = 4;
        
        auto buffer2 = BufferFactory::makeDefaultBuffer(cap);
        assert(len == buffer2.writeFromBuffer(buffer1, skipBytes, len));
        assert(buffer2.size() == len);
        assert(std::memcmp(buffer2.posToRead(), data.substr(skipBytes, len).c_str(), len) == 0);
        
        std::cout << "= writeFromBuffer 1 pass" << std::endl;
    }
    
    //writeFromBuffer 2
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer1 = BufferFactory::makeDefaultBuffer(cap);
        assert(data.length() == buffer1.write(data.c_str(), data.length()));
        
        const uint32_t skipBytes = 3;
        const uint32_t len = data.length() - skipBytes;
        
        auto buffer2 = BufferFactory::makeDefaultBuffer(cap);
        assert(len == buffer2.writeFromBuffer(buffer1, skipBytes, data.length()));
        assert(buffer2.size() == len);
        assert(std::memcmp(buffer2.posToRead(), data.substr(skipBytes, len).c_str(), len) == 0);
        
        std::cout << "= writeFromBuffer 2 pass" << std::endl;
    }
    
    std::cout << "============= complete Buffer test ===============" << std::endl;
}
