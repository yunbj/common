#pragma once


#include <memory>
#include <cstddef>


namespace grid {
    
    enum class MemoryPolicy {
        Default,
        Pool
    };//enum class MemoryPolicy
    
    
    class IBufferData {
    public:
        virtual ~IBufferData() = default;
        
        virtual uint32_t Capacity() const = 0;
        
        virtual const void* Data() const = 0;
        
        virtual void* Data() = 0;
        
        virtual MemoryPolicy Policy() const = 0;
        
    public:
        static std::shared_ptr<IBufferData> Make(uint32_t n, MemoryPolicy policy);
    };//class IBufferData
    
    
	class Buffer {
    private:
        std::shared_ptr<IBufferData> _data;
        uint32_t _beginPos = 0;
        uint32_t _endPos = 0;

    private:
        Buffer(std::shared_ptr<IBufferData> data, uint32_t beginPos, uint32_t endPos);
        
	public:
        Buffer(uint32_t n, MemoryPolicy memPolicy);
        
        Buffer(const void* data, uint32_t size, MemoryPolicy memPolicy);
        
        uint32_t Capacity() const;

        //length of data
		uint32_t Size() const;

        //available bytes to write
        uint32_t Available() const;
        
        //same to posToRead
        const void* Data() const;
        
        void* Data();
        
        //return memory pointer
        const void* Memory() const;
        
        void* Memory();

        const void* PosToRead() const;
        
        void* PosToRead();
        
        const void* PosToWrite() const;
        
        void* PosToWrite();
        
        //begin pos +- n. it can't be over endPos
        void SkipPosToRead(int32_t n);
        
        //end pos +- n. it cant'be less beginPos
        void SkipPosToWrite(int32_t n);
        
        void AdjustToSize(uint32_t n);
        
        //return actually written bytes
        uint32_t Written(uint32_t n);
        
		//endPos will be moved automatically after writting
        //return actually written bytes
		uint32_t Write(const void* data, uint32_t n);
        
        //pos가 end pos보다 크면 skip후 write
        //pos가 end pos보다 작으면 overwrite 후 end pos 조정
        uint32_t WriteAtPos(const void* data, uint32_t n, uint32_t pos);
        
        uint32_t WriteFromBuffer(const Buffer& buffer, uint32_t pos, uint32_t n);
        
        //shallow copy
        Buffer Duplicate() const;
        
        Buffer Duplicate(uint32_t pos, uint32_t n) const;
        
        //deep copy
        Buffer Clone() const;
	};//class Buffer


	class BufferFactory {
	public:
		static Buffer MakeDefaultBuffer(uint32_t n);

        static Buffer MakePoolBuffer(uint32_t n);
    };//class BufferFactory
}//namespace grid

