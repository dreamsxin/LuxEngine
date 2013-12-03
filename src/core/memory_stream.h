#pragma once

#include "core/lux.h"
#include "core/istream.h"
#include "core/vector.h"

namespace Lux
{

	class LUX_CORE_API MemoryStream : public IStream
	{
		public:
			MemoryStream();

			void create(void* data, int size) { m_data = data; m_size = size; m_pos = 0; }
			virtual void write(const void* data, size_t size);
			virtual bool read(void* data, size_t size);
			const char* getBuffer() const { return &m_buffer[0]; }
			int getBufferSize() const { return m_size; }
			void flush() { m_size = 0; }
			void clearBuffer() { m_buffer.clear(); m_pos = 0; m_size = 0; }

			template <class T>
			void write(T value) { write(&value, sizeof(T)); }
			template <class T>
			void read(T& value) { read(&value, sizeof(T)); }
			void rewindForRead() { m_pos = 0; m_data = &m_buffer[0]; m_size = m_buffer.size(); }


		private:
			vector<char> m_buffer;
			int m_pos;
			int m_size;
			void* m_data; 
	};

} // !namespace Lux