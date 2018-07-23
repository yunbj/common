#include "TestBuffer.h"
#include "base/Buffer.h"

#include <string>
#include <iostream>
#include <map>
#include <algorithm>

using namespace grid;


void TestBuffer::DoTest()
{
    std::cout << "============= start Buffer test ===============" << std::endl;
    
	const uint32_t cap = 23;
	const std::string data = "0123456789";

	auto buffer = BufferFactory::makeDefaultBuffer(cap);
	assert(buffer.capacity() == cap);

    //write 1
    assert(data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == data.length());
    assert(memcmp(buffer.data(), (data + data).c_str(), data.length()) == 0);
    assert(buffer.available() == cap - data.length());
    
    //write 2
    assert(data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == data.length()*2);
    assert(memcmp(buffer.data(), (data + data).c_str(), data.length()*2) == 0);
    assert(buffer.available() == cap - data.length()*2);

    //write 3
    const auto resultData = data + data + data.substr(0, cap % data.length());
    
    assert(cap % data.length() == buffer.write(data.c_str(), data.length()));
    
    assert(buffer.size() == cap);
    assert(memcmp(buffer.data(), resultData.c_str(), resultData.length()) == 0);
    assert(buffer.available() == 0);
    
    //skip bytes
    const uint32_t skipBytes = 3;
    const auto resultData2 = resultData.substr(skipBytes);
    
    buffer.skipBytes(skipBytes);
    assert(buffer.size() == cap - skipBytes);
    assert(memcmp(buffer.data(), resultData2.c_str(), resultData2.length()) == 0);

    //duplicate(shallow copy)
    auto bufferDup = buffer.duplicate();
    assert(bufferDup.data() == buffer.data());
    assert(bufferDup.size() == buffer.size());
    assert(memcmp(bufferDup.data(), buffer.data(), buffer.size()) == 0);

    //copy ctor(shallow copy)
    auto bufferCtor = buffer;
    assert(bufferCtor.data() == buffer.data());
    assert(bufferCtor.size() == buffer.size());
    assert(memcmp(bufferCtor.data(), buffer.data(), buffer.size()) == 0);

    //clone(deep copy)
    auto bufferClone = buffer.clone();
    assert(bufferClone.data() != buffer.data());
    assert(bufferClone.size() == buffer.size());
    assert(memcmp(bufferClone.data(), buffer.data(), buffer.size()) == 0);

    std::cout << "============= complete Buffer test ===============" << std::endl;
}
