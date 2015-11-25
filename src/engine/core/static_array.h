#pragma once


namespace Lumix
{
	template <class T, size_t Size> 
	class StaticArray
	{
	public:
		typedef T								value_type;
		typedef StaticArray<value_type, Size>	my_type;
		typedef int32							size_type;

	public:
		enum { elementSize = sizeof(value_type) };

		void assign(const value_type& val)
		{
			for(size_type i = 0; i < Size; ++i)
			{
				m_a[i] = val;
			}
		}

		LUMIX_FORCE_INLINE size_type size() const 
		{
			return Size;
		}

		LUMIX_FORCE_INLINE size_type maxSize() const 
		{
			return Size;
		}

		bool empty() const 
		{
			return 0 == Size;
		}

		value_type& at(size_type i) 
		{
			ASSERT(i >= 0 && i < Size);
			return m_a[i];
		}

		const value_type& at(size_type i) const 
		{
			ASSERT(i >= 0 && i < Size);
			return m_a[i];
		}

		value_type& operator[](size_type i) 
		{
			ASSERT(i >= 0 && i < Size);
			return m_a[i];
		}

		const value_type& operator[](size_type i) const 
		{
			ASSERT(i >= 0 && i < Size);
			return m_a[i];
		}

		value_type& front() 
		{
			return m_a[0];
		}

		const value_type& front() const 
		{
			return m_a[0];
		}

		value_type& back() 
		{
			return m_a[Size - 1];
		}

		const value_type& back() const 
		{
			return m_a[Size - 1];
		}

		size_type find(size_type from, size_type to, const value_type& val) const
		{
			ASSERT(size() >= to);
			for (size_type i = from; i < to; ++i)
			{
				if (m_a[i] == val)
					return i;
			}

			return(-1);
		}

		size_type find(const value_type& val) const
		{
			return find(0, size(), val);
		}

		void swap(size_type idx1, size_type idx2)
		{
			ASSERT(idx1 < Size && idx2 < Size);

			if (idx1 != idx2)
			{
				value_type tmp = m_a[idx1];
				m_a[idx1] = m_a[idx2];
				m_a[idx2] = tmp;
			}
		}

		value_type* data() 
		{ 
			return m_a;
		}

		const value_type* data() const
		{
			return m_a;
		}

	private:
		value_type m_a[Size];
	};
	// not supported
	template <class T> class StaticArray<T, 0>;
} // ~namespace Lumix
