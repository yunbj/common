#pragma once


#include <memory>
#include <cstddef>


namespace grid
{
    class IBufferData
    {
    public:
        virtual ~IBufferData() = default;
        
        virtual uint32_t capacity() const = 0;
        
        virtual const void* data() const = 0;
        
        virtual void* data() = 0;
    };//class IBufferData
    
    
	class Buffer
	{
    private:
        std::shared_ptr<IBufferData> _data;
        uint32_t _beginPos = 0;
        uint32_t _endPos = 0;

    public:
        enum class MemoryPolicy
        {
            Default,
            Pool
        };//enum class Polic

    private:
        void* _getEndPos() const;
        
	public:
        Buffer(uint32_t n, MemoryPolicy memPolicy);
        
        Buffer(const void* data, uint32_t size, MemoryPolicy memPolicy);
        
        uint32_t capacity() const;

        //length of data
		uint32_t size() const;

        //available bytes to write
        uint32_t available() const;
        
		const void* data() const;

        void* data();
        
        //begin index plus n
        void skipBytes(uint32_t n);
        
		//endPos will be moved automatically after writting
		uint32_t write(const void* data, uint32_t n);
        
        //shallow copy
        Buffer duplicate() const;
        
        //deep copy
        Buffer clone() const;
	};//class Buffer


	class BufferFactory
	{
	public:
		static Buffer makeDefaultBuffer(uint32_t n);

        static Buffer makePoolBuffer(uint32_t n);
    };//class BufferFactory
}//namespace grid

