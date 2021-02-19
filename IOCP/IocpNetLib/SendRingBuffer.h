#pragma once

#include "CommonDefine.h"
#include "Lock.h"


namespace NetLib
{
	class SendRingBuffer
	{
	public:
		SendRingBuffer() = default;

		~SendRingBuffer()
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

			m_pBeginMark = m_pRingBuffer;
			m_pEndMark = m_pBeginMark + m_RingBufferSize - 1;

			Init();
			return true;
		}

		void Init()
		{
			SpinLockGuard Lock(&m_CS);

			m_UsedBufferSize = 0;

			m_pCurMark = m_pBeginMark;
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark = m_pEndMark;
		}

		char* GetBuffer(const int reqReadSize, OUT int& resReadSize)
		{
			SpinLockGuard Lock(&m_CS);

			char* pResult = nullptr;
			if (m_pLastMoveMark == m_pGettedBufferMark)
			{
				m_pGettedBufferMark = m_pBeginMark;
				m_pLastMoveMark = m_pEndMark;
			}

			if (m_UsedBufferSize > reqReadSize)
			{
				if ((m_pLastMoveMark - m_pGettedBufferMark) >= reqReadSize)
				{
					resReadSize = reqReadSize;
				}
				else
				{
					resReadSize = static_cast<int>(m_pLastMoveMark - m_pGettedBufferMark);
				}

				pResult = m_pGettedBufferMark;
				m_pGettedBufferMark += resReadSize;
			}
			else if (m_UsedBufferSize > 0)
			{
				if ((m_pLastMoveMark - m_pGettedBufferMark) >= m_UsedBufferSize)
				{
					resReadSize = m_UsedBufferSize;
					pResult = m_pGettedBufferMark;
					m_pGettedBufferMark += m_UsedBufferSize;
				}
				else
				{
					resReadSize = static_cast<int>(m_pLastMoveMark - m_pGettedBufferMark);
					pResult = m_pGettedBufferMark;
					m_pGettedBufferMark += resReadSize;
				}
			}

			return pResult;
		}

		char* ForwardMark(const int forwardLength)
		{
			SpinLockGuard Lock(&m_CS);

			char* pPreCurMark = nullptr;
			if (m_UsedBufferSize + forwardLength > m_RingBufferSize)
			{
				return pPreCurMark;
			}

			if ((m_pEndMark - m_pCurMark) >= forwardLength)
			{
				pPreCurMark = m_pCurMark;
				m_pCurMark += forwardLength;
			}
			else
			{
				m_pLastMoveMark = m_pCurMark;
				m_pCurMark = m_pBeginMark + forwardLength;
				pPreCurMark = m_pBeginMark;
			}

			return pPreCurMark;
		}

		char* ForwardMark(const int forwardLength, const int nextLength, const DWORD remainLength)
		{
			SpinLockGuard Lock(&m_CS);

			if (m_UsedBufferSize + forwardLength + nextLength > m_RingBufferSize)
			{
				return nullptr;
			}

			if ((m_pEndMark - m_pCurMark) >= (nextLength + forwardLength))
			{
				m_pCurMark += forwardLength;
			}
			else
			{
				m_pLastMoveMark = m_pCurMark;

				CopyMemory(m_pBeginMark, m_pCurMark - (remainLength - forwardLength), remainLength);
				m_pCurMark = m_pBeginMark + remainLength;
			}

			return m_pCurMark;
		}

		void ReleaseBuffer(const int releaseSize)
		{
			SpinLockGuard Lock(&m_CS);
			m_UsedBufferSize = (releaseSize > m_UsedBufferSize) ? 0 : (m_UsedBufferSize - releaseSize);
		}

		void SetUsedBufferSize(const int usedBufferSize)
		{
			SpinLockGuard Lock(&m_CS);
			m_UsedBufferSize += usedBufferSize;
		}

		int GetBufferSize() { return m_RingBufferSize; }
		char* GetBeginMark() { return m_pBeginMark; }
		char* GetCurMark() { return m_pCurMark; }


	private:
		CustomSpinLockCriticalSection m_CS;

		int m_RingBufferSize = INVALID_VALUE;
		int m_UsedBufferSize = INVALID_VALUE;
		char* m_pRingBuffer = nullptr;
		char* m_pBeginMark = nullptr;
		char* m_pEndMark = nullptr;
		char* m_pCurMark = nullptr;
		char* m_pGettedBufferMark = nullptr;
		char* m_pLastMoveMark = nullptr;
	};
}