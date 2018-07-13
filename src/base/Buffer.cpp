#include "base/Buffer.h"


#include <cassert>
#include <stdexcept>


namespace grid
{


class DefaultBuffer : public IBuffer
{
private:
	uint8_t* _data;
	const std::size_t _capacity;
	std::size_t _size;

public:
	explicit DefaultBuffer(std::size_t n)
		: _data(nullptr)
		, _capacity(n)
		, _size(0)
	{
		assert(n > 0);

		_data = new uint8_t[n];
	}

	~DefaultBuffer()
	{
		if (_data)
		{
			delete[] _data;
			_data = nullptr;
		}
	}

	virtual std::size_t capacity() const
	{
		return _capacity;
	}

	virtual void resize(std::size_t n)
	{
		if (n <= _capacity)
		{
			_size = n;
		}
		else
		{
			throw std::runtime_error("buffer over capacity");
		}
	}

	virtual std::size_t size() const
	{
		return _size;
	}

	virtual void* data() const
	{
		return _data;
	}
};//class IBuffer


std::shared_ptr<IBuffer> BufferFactory::makeDefaultBuffer(std::size_t n)
{
	return std::make_shared<DefaultBuffer>(n);
}

std::shared_ptr<IBuffer> BufferFactory::makePoolBuffer(std::size_t n)
{
    return makeDefaultBuffer(n);
}

    
}//namespace grid

