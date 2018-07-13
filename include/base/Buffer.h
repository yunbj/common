#pragma once


#include <memory>
#include <cstddef>


namespace grid
{
	class IBuffer
	{
	public:
		virtual ~IBuffer() = default;

		virtual std::size_t capacity() const = 0;

		virtual void resize(std::size_t n) = 0;

		virtual std::size_t size() const = 0;

		virtual void* data() const = 0;
	};//class IBuffer


	class BufferFactory
	{
	public:
		static std::shared_ptr<IBuffer> makeDefaultBuffer(std::size_t n);

        static std::shared_ptr<IBuffer> makePoolBuffer(std::size_t n);
    };//class BufferFactory
}//namespace grid

