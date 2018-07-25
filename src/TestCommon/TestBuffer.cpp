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
    
    //write manually
    //assert(data.length() == buffer.write(data.c_str(), data.length()));
    std::memcpy(buffer.data(), data.c_str(), data.length());
    assert(buffer.written(data.length()) == data.length());
    
    assert(buffer.size() == data.length());
    assert(std::memcmp(buffer.data(), (data + data).c_str(), data.length()) == 0);
    assert(buffer.available() == cap - data.length());
    
    std::cout << "= manual write pass" << std::endl;
    
    //write via method when sufficient memory
    assert(data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == data.length()*2);
    assert(std::memcmp(buffer.data(), (data + data).c_str(), data.length()*2) == 0);
    assert(buffer.available() == cap - data.length()*2);

    std::cout << "= write method pass" << std::endl;
    
    //write via method when not sufficient memory
    const auto resultData = data + data + data.substr(0, cap % data.length());
    
    assert(cap % data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == cap);
    assert(std::memcmp(buffer.data(), resultData.c_str(), resultData.length()) == 0);
    assert(buffer.available() == 0);

    std::cout << "= write() when not sufficient memory pass" << std::endl;
    
    //skip
    const uint32_t skipBytes = 3;

        //skipPosToRead +
    {
        const auto skippedData = resultData.substr(skipBytes);
    
        buffer.skipPosToRead(skipBytes);
        assert(buffer.size() == skippedData.length());
        assert(std::memcmp(buffer.data(), skippedData.c_str(), skippedData.length()) == 0);

        std::cout << "= skipPosToRead(+) pass" << std::endl;
    }
    
        //skipPosToRead -
    {
        buffer.skipPosToRead(-skipBytes);
        assert(buffer.size() == resultData.length());
        assert(std::memcmp(buffer.data(), resultData.c_str(), resultData.length()) == 0);

        std::cout << "= skipPosToRead(-) pass" << std::endl;
    }
    
        //skipPosToWrite -
    {
        const auto skippedData = resultData.substr(0, resultData.length() - 3);
        
        buffer.skipPosToWrite(-skipBytes);
        assert(buffer.size() == skippedData.length());
        assert(std::memcmp(buffer.data(), skippedData.c_str(), skippedData.length()) == 0);

        std::cout << "= skipPosToWrite(-) pass" << std::endl;
    }
    
        //skipPosToWrite +
    {
        buffer.skipPosToWrite(skipBytes);

        assert(buffer.size() == resultData.length());
        assert(std::memcmp(buffer.data(), resultData.c_str(), resultData.length()) == 0);

        std::cout << "= skipPosToWrite(+) pass" << std::endl;
    }
    
    //duplicate(shallow copy)
    auto bufferDup = buffer.duplicate();
    assert(bufferDup.data() == buffer.data());
    assert(bufferDup.size() == buffer.size());
    assert(std::memcmp(bufferDup.data(), buffer.data(), buffer.size()) == 0);

    std::cout << "= duplicate() pass" << std::endl;

    //copy ctor(shallow copy)
    auto bufferCtor = buffer;
    assert(bufferCtor.data() == buffer.data());
    assert(bufferCtor.size() == buffer.size());
    assert(std::memcmp(bufferCtor.data(), buffer.data(), buffer.size()) == 0);

    std::cout << "= ctor() pass" << std::endl;
    
    //clone(deep copy)
    auto bufferClone = buffer.clone();
    assert(bufferClone.data() != buffer.data());
    assert(bufferClone.size() == buffer.size());
    assert(std::memcmp(bufferClone.data(), buffer.data(), buffer.size()) == 0);

    std::cout << "= clone() pass" << std::endl;
    
    //writeAtPos()
    {
        const uint32_t cap = 100;
        const std::string data = "0123456789";
        
        auto buffer = BufferFactory::makeDefaultBuffer(cap);
        assert(buffer.capacity() == cap);
        
        assert(data.length() == buffer.write(data.c_str(), data.length()));
        assert(std::memcmp(buffer.data(), data.c_str(), data.length()) == 0);
        assert(buffer.size() == data.length());
        assert(buffer.available() == cap - data.length());

        // pos is over size
        const uint32_t gap = 4;
        uint32_t pos = data.length() + gap;
        assert(data.length() == buffer.writeAtPos(data.c_str(), data.length(), pos));
        assert(buffer.size() == pos + data.length());
        assert(std::memcmp(buffer.data(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.data()+pos, data.c_str(), data.length()) == 0);

        //pos is less size & post size is less size
        const std::string zero = "0000000000";
        const std::string zeroAndTail = zero + data.substr(data.length() - gap);
        pos = data.length();
        const auto prevSize = buffer.size();
        assert(data.length() == buffer.writeAtPos(zero.c_str(), zero.length(), pos));
        assert(prevSize == buffer.size());
        assert(std::memcmp(buffer.data(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.data()+data.length(), zeroAndTail.c_str(), zeroAndTail.length()) == 0);

        //pos is less size & post size is over size
        const std::string zero2 = zero + zero;
        pos = data.length() * 2;
        assert(data.length() == buffer.writeAtPos(zero.c_str(), zero.length(), pos));
        assert(pos + zero.length() == buffer.size());
        assert(std::memcmp(buffer.data(), data.c_str(), data.length()) == 0);
        assert(std::memcmp((uint8_t*)buffer.data()+data.length(), zero2.c_str(), zero2.length()) == 0);

        std::cout << "= writeAtPos() pass" << std::endl;
    }
    
    std::cout << "============= complete Buffer test ===============" << std::endl;
}
