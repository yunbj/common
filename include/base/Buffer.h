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

	public:
        Buffer(uint32_t n, MemoryPolicy memPolicy);
        
        Buffer(const void* data, uint32_t size, MemoryPolicy memPolicy);
        
        uint32_t capacity() const;

        //length of data
		uint32_t size() const;

        //available bytes to write
        uint32_t available() const;
        
        //same to posToRead
        const void* data() const;
        
        void* data();
        
        const void* posToRead() const;
        
        void* posToRead();
        
        const void* posToWrite() const;
        
        void* posToWrite();
        
        //begin index plus n. it can't be over endPos
        void skipFront(uint32_t n);
        
        //end index minus n. it cant'be less beginPos
        void skipBack(uint32_t n);
        
        //return actually written bytes
        uint32_t written(uint32_t n);
        
		//endPos will be moved automatically after writting
        //return actually written bytes
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

