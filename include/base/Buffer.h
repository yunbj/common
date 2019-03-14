#pragma once


#include <memory>
#include <cstddef>
#include <string>


namespace grid
{
    class IBufferData
    {
    public:
        virtual ~IBufferData() = default;
        
        virtual uint32_t Capacity() const = 0;
        
        virtual const void* Data() const = 0;
        
        virtual void* Data() = 0;
        
        virtual std::shared_ptr<IBufferData> Make(uint32_t size) = 0;
    };//class IBufferData
    
    
	class Buffer {
    private:
        std::shared_ptr<IBufferData> _data;
        uint32_t _beginPos = 0;
        uint32_t _endPos = 0;

    private:
        friend class BufferFactory;
        
        explicit Buffer(std::shared_ptr<IBufferData> data, uint32_t beginPos = 0, uint32_t endPos = 0);
        
        Buffer(std::shared_ptr<IBufferData> data, const void* ptr, uint32_t size);
        
	public:
        Buffer(const Buffer& rhs);

        Buffer& operator=(const Buffer& rhs);

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
		uint32_t Write(const void* ptr, uint32_t n);
        
        //pos가 end pos보다 크면 skip후 write
        //pos가 end pos보다 작으면 overwrite 후 end pos 조정
        uint32_t WriteAtPos(const void* ptr, uint32_t n, uint32_t pos);
        
        uint32_t WriteFromBuffer(const Buffer& buffer, uint32_t pos, uint32_t n);
        
        //shallow copy
        Buffer Duplicate() const;
        
        Buffer Duplicate(uint32_t pos, uint32_t n) const;
        
        //deep copy
        Buffer Clone() const;

	public:
        bool operator==(const Buffer& rhs) const;
	};//class Buffer


	class BufferFactory {
	public:
		static Buffer MakeDefaultBuffer(uint32_t n);

        static Buffer MakePoolBuffer(uint32_t n);
        
        static Buffer MakeFixedSizePoolBuffer(uint32_t n);

	public:
        static Buffer MakeDefaultBuffer(const void* ptr, uint32_t n);

        static Buffer MakePoolBuffer(const void* ptr, uint32_t n);

        static Buffer MakeFixedSizePoolBuffer(const void* ptr, uint32_t n);

	public:
        static Buffer MakeDefaultBuffer(const std::string& str);

        static Buffer MakePoolBuffer(const std::string& str);

        static Buffer MakeFixedSizePoolBuffer(const std::string& str);
    };//class BufferFactory
}//namespace grid

