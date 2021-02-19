#pragma once

#include "CommonDefine.h"
#include "Lock.h"


namespace NetLib
{
	class RecvRingBuffer
	{
	public:
		RecvRingBuffer() = default;

		~RecvRingBuffer()
		{ 
			delete[] m_pRingBuffer;
		}

	
		bool Create(const int ringBufferSize)
		{
			if (ringBufferSize == INVALID_VALUE)
			{
				return false;
			}

			m_RingBufferSize = ringBufferSize;

			delete[] m_pRingBuffer;
			m_pRingBuffer = new char[m_RingBufferSize];

			Init();
			return true;
		}

		void Init()
		{
			m_ReadPos = 0;
			m_WritePos = 0;			
		}

		char* GetWriteBuffer(const int wantedSize)
		{
			if (auto size = m_RingBufferSize - m_WritePos; size < wantedSize)
			{
				auto remain = m_WritePos - m_ReadPos;
				if (remain > 0)
				{
					CopyMemory(&m_pRingBuffer[0], &m_pRingBuffer[m_ReadPos], remain);
				}

				m_WritePos = 0;
				m_ReadPos = 0;
			}

			return &m_pRingBuffer[m_WritePos];			
		}

		std::tuple<int, char*> GetReceiveData(int recvSize)
		{
			m_WritePos += recvSize;

			auto remain = m_WritePos - m_ReadPos;
			return { remain, &m_pRingBuffer[m_ReadPos] };
		}

		void UsedRecvBuffer(const int forwardLength)
		{
			m_ReadPos += forwardLength;
		}

		/*int ForwardWritePos(const int forwardLength)
		{
			m_WritePos += forwardLength;

			return m_WritePos - m_ReadPos;
		}*/
						
		//char* GetReadBuffer() { return &m_pRingBuffer[m_ReadPos]; }
		
		int GetBufferSize() { return m_RingBufferSize; }
		

	private:		
		char* m_pRingBuffer = nullptr;
		int m_RingBufferSize = INVALID_VALUE;
		int m_ReadPos = 0;
		int m_WritePos = 0;

		//int m_UsedBufferSize = INVALID_VALUE;		
		//char* m_pBeginMark = nullptr;
		//char* m_pEndMark = nullptr;
		//char* m_pCurMark = nullptr;
		//char* m_pGettedBufferMark = nullptr;
		//char* m_pLastMoveMark = nullptr;
	};
}