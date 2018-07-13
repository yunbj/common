#include "TestBuffer.h"
#include "base/Buffer.h"

#include <string>
#include <iostream>


using namespace grid;


void TestBuffer::DoTest()
{
    std::cout << "============= start Buffer test ===============" << std::endl;
    
	const std::size_t cap = 100;
	const std::string data = "abc";

	auto buffer = BufferFactory::makeDefaultBuffer(cap);

	assert(buffer->capacity() == cap);

	memcpy(buffer->data(), data.c_str(), data.length());
	buffer->resize(data.length());

	assert(buffer->size() == data.length());
	assert(memcmp(buffer->data(), data.c_str(), data.length()) == 0);
    
    try
    {
        buffer->resize(cap + 1);
        assert(false);
    }
    catch (std::runtime_error& e)
    {
        assert(true);
    }

    std::cout << "============= complete Buffer test ===============" << std::endl;
}
