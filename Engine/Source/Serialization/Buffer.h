#pragma once
#include "Core/Core.h"


#define STARTREAD(bufferref, startoffset) const Buffer& _buffer = bufferref; size_t m_Stride = startoffset; size_t size = 0; std::vector<char> str;
#define STARTWRITE(bufferref, startoffset) Buffer& _buffer = bufferref; size_t m_Stride = startoffset; size_t size = 0; std::vector<char> str;
#define STOPREAD() return (uint)m_Stride;
#define STOPWRITE() return (uint)m_Stride;

#define WRITE(x, s) if(_buffer.size() < m_Stride + s) _buffer.resize(m_Stride + s); memcpy(_buffer.data() + m_Stride, x, s); m_Stride += s
#define READ(x, s)  memcpy(x, _buffer.data() + m_Stride, s); m_Stride += s

#define WRITEVEC(x, singlesize) size = x.size(); WRITE(&size, sizeof(size)); WRITE(x.data(), size * singlesize)
#define READVEC(x, singlesize) size = 0; READ(&size, sizeof(size)); x.resize(size); READ(x.data(), size * singlesize)
#define WRITESTRING(x) str = std::vector<char>(x.c_str(), x.c_str() + x.length()); WRITEVEC(str, 1)
#define READSTRING(x) READVEC(str, 1); x = std::string(str.begin(), str.end())

#define WRITEBUFFER(buffer) WRITEVEC(buffer, 1)
#define READBUFFER(buffer) READVEC(buffer, 1)


#define MAPBUFFERWRITE(MapRef, Key, WriteFunc) [&](){ Buffer buffer; \
STARTWRITE(buffer, 0); WriteFunc; MapRef.AddPiece(Key, buffer); }();

#define MAPBUFFERREAD(MapRef, Key, ReadFunc) [&](){ if(!MapRef.HasKey(Key)) return; Buffer& buffer = MapRef[Key]; \
STARTREAD(buffer, 0); ReadFunc; }();


using Buffer = std::vector<byte>;

//array of buffers
struct DENGINE_API ArrayBuffer
{
	std::vector<Buffer> m_DataPieces;

	const std::vector<Buffer>& GetDataPieces() const
	{
		return m_DataPieces;
	}

	void AddPiece(const Buffer& piece)
	{
		m_DataPieces.push_back(piece);
	}

	void FromBuffer(const Buffer& buffer)
	{
		uint size = 0;
		uint offset = 0;
		while (offset < buffer.size())
		{
			memcpy(&size, buffer.data() + offset, sizeof(uint));
			offset += sizeof(uint);

			Buffer piece(size);
			memcpy(piece.data(), buffer.data() + offset, size);
			offset += size;
			AddPiece(piece);
		}
	}

	Buffer MakeBuffer()
	{
		Buffer buffer;

		for (auto& piece : m_DataPieces)
		{
			uint size = (uint)piece.size();
			buffer.resize(buffer.size() + sizeof(uint));
			memcpy(buffer.data() + buffer.size() - sizeof(uint), &size, sizeof(uint));

			buffer.insert(buffer.end(), piece.begin(), piece.end());
		}

		return buffer;
	}
};


//map of buffers
struct DENGINE_API MapBuffer
{
	std::unordered_map<std::string, Buffer> m_DataPieces;

	MapBuffer(const Buffer& input)
	{
		FromBuffer(input);
	}

	MapBuffer() 
	{
	
	}

	const std::unordered_map<std::string, Buffer>& GetDataPieces() const
	{
		return m_DataPieces;
	}

	void AddPiece(const std::string& key, const Buffer& piece)
	{
		m_DataPieces[key] = piece;
	}

	bool HasKey(const std::string& key)
	{
		return (m_DataPieces.count(key));
	}

	void QuickWrite(const std::string& key, std::function<void()> WriteFunc)
	{
		Buffer buffer;
		STARTWRITE(buffer, 0);
		WriteFunc();
		AddPiece(key, buffer);
	}

	void QuickRead(const std::string& key, std::function<void()> ReadFunc)
	{
		const Buffer& buffer = m_DataPieces[key];
		STARTREAD(buffer, 0);
		ReadFunc();
	}


	Buffer& operator[](const std::string& key) 
	{
		return m_DataPieces[key];
	}

	void FromBuffer(const Buffer& buffer)
	{
		ArrayBuffer PieceBuffer;
		PieceBuffer.FromBuffer(buffer);

		for(const auto& piece : PieceBuffer.GetDataPieces())
		{
			std::string Key;
			Buffer Data;
			STARTREAD(piece, 0);
			READSTRING(Key);
			READBUFFER(Data);

			AddPiece(Key, Data);		
		}
	}

	Buffer MakeBuffer()
	{
		ArrayBuffer PieceBuffer;
		for (auto& piece : m_DataPieces)
		{
			Buffer BufferPiece;
			STARTWRITE(BufferPiece, 0);
			WRITESTRING(piece.first);
			WRITEBUFFER(piece.second);

			PieceBuffer.AddPiece(BufferPiece);
		}

		return PieceBuffer.MakeBuffer();
	}
};



#if 0 
#define STARTREAD(bufferref, startoffset) const Buffer& _buffer = bufferref; uint m_Stride = startoffset; uint size = 0; std::vector<char> str;
#define STARTWRITE(bufferref, startoffset) Buffer& _buffer = bufferref; uint m_Stride = startoffset; uint size = 0; std::vector<char> str;
#define STOPREAD() return m_Stride;
#define STOPWRITE() return m_Stride;

#define WRITE(x, s) _buffer.Append(x, s);  m_Stride += s
#define READ(x, s)  memcpy(x, (byte*)_buffer.Data + m_Stride, s); m_Stride += s

#define WRITEVEC(x, singlesize) size = x.size(); WRITE(&size, sizeof(size)); WRITE(x.data(), size * singlesize)
#define READVEC(x, singlesize) size = 0; READ(&size, sizeof(size)); x.resize(size); READ(x.data(), size * singlesize)
#define WRITESTRING(x) str = std::vector<char>(x.c_str(), x.c_str() + x.length()); WRITEVEC(str, 1)
#define READSTRING(x) READVEC(str, 1); x = std::string(str.begin(), str.end())

#define WRITEBUFFER(OtherBuffer) size = OtherBuffer.GetSize(); WRITE(&size, sizeof(size)); WRITE(OtherBuffer.Data, size * 1)
#define READBUFFER(OtherBuffer) size = 0; READ(&size, sizeof(size)); OtherBuffer.Allocate(size); READ(OtherBuffer.Data, size * 1)


//array of bytes
struct Buffer
{
	byte* Data = nullptr;
	uint Size = 0;

	Buffer() : Data(nullptr), Size(0)
	{
	}

	Buffer(const Buffer& other)
	{
		Allocate(other.GetSize());
		memcpy(Data, other.Data, other.GetSize());
	}

	Buffer(Buffer&& other) noexcept
	{
		Allocate(other.GetSize());
		memcpy(Data, other.Data, other.GetSize());
	}

	Buffer(const std::vector<char>& vec) : Data((byte*)vec.data()), Size(vec.size())
	{
	}

	~Buffer()
	{
		delete[] Data;
	}

	Buffer(uint size)
	{
		Allocate(size);
	}

	Buffer(void* data, uint size) : Data((byte*)data), Size(size)
	{
	}
	
	const byte* ConstData() const
	{
		return (byte*)Data;
	}

	byte* MutableData()
	{
		return (byte*)Data;
	}

	void Append(const Buffer& other)
	{
		Append(other.Data, other.GetSize());
	}

	//append a buffer to the end of this buffer
	void Append(const void* data, uint size)
	{
		//copy all my current data 
		uint lastSize = GetSize();
		byte* lastData = new byte[lastSize];
		memcpy(lastData, Data, lastSize);

		//allocate new space
		Allocate(lastSize + size);

		//get old data back
		memcpy(Data, lastData, lastSize);

		//append new data
		memcpy(Data + lastSize, data, size);
	}

	void Allocate(uint size)
	{
		delete[] Data;
		Data = nullptr;

		if (size == 0)
			return;

		Data = new byte[size];
		Size = size;
	}

	void Release()
	{
		delete[] Data;
		Data = nullptr;
		Size = 0;
	}

	void ZeroInitialize()
	{
		if (Data)
			memset(Data, 0, Size);
	}

	template<typename T>
	T& Read(uint offset = 0)
	{
		return *(T*)((byte*)Data + offset);
	}

	byte* ReadBytes(uint size, uint offset)
	{
		
		ERROR(offset + size <= Size, "Buffer overflow!");
		byte* buffer = new byte[size];
		memcpy(buffer, (byte*)Data + offset, size);
		return buffer;
	}

	void Write(void* data, uint size, uint offset = 0)
	{
		ERROR(offset + size <= Size, "Buffer overflow!");
		memcpy((byte*)Data + offset, data, size);
	}

	operator bool() const
	{
		return Data;
	}

	byte& operator[](int index)
	{
		return ((byte*)Data)[index];
	}

	byte operator[](int index) const
	{
		return ((byte*)Data)[index];
	}

	template<typename T>
	T* As()
	{
		return (T*)Data;
	}

	inline uint GetSize() const { return Size; }
};

struct ArrayBuffer
{
	std::vector<Buffer> m_DataPieces;

	void AddPiece(const Buffer& piece)
	{
		m_DataPieces.push_back(piece);
	}

	void FromBuffer(const Buffer& buffer)
	{
		uint size = 0;
		uint offset = 0;
		while (offset < buffer.GetSize())
		{
			memcpy(&size, buffer.ConstData() + offset, sizeof(uint));
			offset += sizeof(uint);

			Buffer piece(size);
			memcpy(piece.MutableData(), buffer.ConstData() + offset, size);
			offset += size;
			AddPiece(piece);
		}
	}

	Buffer MakeBuffer()
	{
		Buffer buffer;
		uint FinalSize = 0;
		for (auto& piece : m_DataPieces)
		{
			FinalSize += (uint)piece.GetSize();
			FinalSize += sizeof(uint);//account for the piece size
		}

		buffer.Allocate(FinalSize);

		STARTWRITE(buffer, 0);
		for (auto& piece : m_DataPieces)
		{
			//write the size
			uint size = piece.GetSize();
			WRITE(&size, sizeof(uint));

			//write the actual piece
			WRITE(piece.Data, piece.GetSize());	
		}
		STOPWRITE();

		return buffer;
	}
};
#endif