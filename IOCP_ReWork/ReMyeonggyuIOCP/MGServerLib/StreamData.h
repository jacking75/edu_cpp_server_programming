#pragma once

#include <windows.h>

const size_t StreamDataSize = 64000;

class StreamData
{
public:
	StreamData()
	{
		reset();
	}

	virtual ~StreamData() = default;
	

public:	
	virtual StreamData* clone()
	{
		StreamData * o = new StreamData();
		CopyMemory(o->m_Value, this->m_Value, StreamDataSize);
		o->m_SizeOfReceived = m_SizeOfReceived;
		o->m_Capacity = m_Capacity;
		return o;
	}

	char* current() 
	{ 
		return m_Value+m_SizeOfReceived; 
	}

	void increaseSize(int sz) 
	{
		m_SizeOfReceived += sz; 
	}

	char* getStream() 
	{ 
		return m_Value;
	}

	unsigned int getSize() 
	{ 
		return m_SizeOfReceived;
	}

	unsigned int getCapacity() 
	{ 
		return m_Capacity;
	}

	unsigned int availableSize() 
	{
		return m_Capacity-m_SizeOfReceived;
	}

	void reset()
	{
		m_SizeOfReceived = 0;
		ZeroMemory(m_Value, sizeof(m_Value));
	}

	void flushTo(unsigned int sz)
	{
		if (m_SizeOfReceived > sz)
		{
			memmove(m_Value,m_Value+sz,m_SizeOfReceived-sz);
			m_SizeOfReceived-=sz;
		}
		else 
		{
			m_SizeOfReceived = 0;
		}
	}

	unsigned int putData(unsigned int size, char* datas)
	{
		if (0 >= size) {
			return 0;
		}

		if (availableSize() < size) {
			return 0;
		}

		CopyMemory(current(), datas, size);
		increaseSize(size);
		return size;
	}	



protected:
	char	m_Value[StreamDataSize];

	unsigned int	m_Capacity = StreamDataSize;

	unsigned int	m_SizeOfReceived = 0;
};




