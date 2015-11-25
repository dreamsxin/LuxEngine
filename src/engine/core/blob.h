#pragma once


#include "lumix.h"
#include "core/array.h"


namespace Lumix
{

	class LUMIX_ENGINE_API OutputBlob
	{
		public:
			explicit OutputBlob(IAllocator& allocator);
			OutputBlob(const OutputBlob& blob, IAllocator& allocator);
			void operator =(const OutputBlob& rhs);
			OutputBlob(const OutputBlob& rhs);

			void reserve(int size) { m_data.reserve(size); }
			const void* getData() const { return m_data.empty() ? nullptr : &m_data[0]; }
			int getSize() const { return m_data.size(); }
			void write(const void* data, int size);
			void writeString(const char* string);
			template <class T> void write(const T& value) { write(&value, sizeof(T)); }
			template <> void write<bool>(const bool& value) { uint8 v = value; write(&v, sizeof(v)); }
			void clear() { m_data.clear(); }

			OutputBlob& operator << (const char* str);
			OutputBlob& operator << (int value);
			OutputBlob& operator << (uint32 value);
			OutputBlob& operator << (float value);

		private:
			Array<uint8> m_data;
	};


	class LUMIX_ENGINE_API InputBlob
	{
		public:
			InputBlob(const void* data, int size);
			InputBlob(const OutputBlob& blob);

			bool read(void* data, int size);
			bool readString(char* data, int max_size);
			template <class T> void read(T& value) { read(&value, sizeof(T)); }
			template <class T> T read() { T v; read(&v, sizeof(v)); return v; }
			template <> bool read<bool>() { uint8 v; read(&v, sizeof(v)); return v != 0; }
			const void* getData() const { return (const void*)m_data; }
			int getSize() const { return m_size; }
			void setPosition(int pos) { m_pos = pos; }
			void rewind() { m_pos = 0; }


		private:
			const uint8* m_data;
			int m_size;
			int m_pos;
	};

} // !namespace Lumix
