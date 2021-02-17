
#include "CircularQueue.h"
#include <Windows.h>

namespace COMMONLIB
{
	CircularQueue::CircularQueue()
	{
		memset(cRingBuf, 0, sizeof(cRingBuf));

		cpBufEnd = cRingBuf + sizeof(cRingBuf) - 1; //버퍼 넘어서는 것을 체크하기 위해 미리 계산해 둠

		Clear();
	}

	CircularQueue::~CircularQueue()
	{
		
	}

	void CircularQueue::Clear()
	{
		cpBufFront = cRingBuf;
		cpBufRear = cRingBuf;
		dwDataSize = 0;
	}

	bool CircularQueue::IsBufferEmpty()
	{
		return (cpBufFront == cpBufRear);
	}

	bool CircularQueue::IsBufferFull()
	{
		return (dwDataSize == GetQueueSize());
	}

	bool CircularQueue::Push(const char* pData, int const nSize)
	{
		if (nSize > GetRemainQueueSize()) //큐 여분 보다 크면 false
			return false;

		if (nSize <= (cpBufEnd - cpBufRear - 1))
		{
			//한번에 다 쓸수 있는 경우
			memcpy(cpBufRear, pData, nSize);
			cpBufRear += nSize;
		}
		else
		{
			//(1) 버퍼끝까지 쓰고
			int nTempSize = cpBufEnd - cpBufRear;
			memcpy(cpBufRear, pData, nTempSize);
			cpBufRear = cRingBuf; //버퍼 제일 앞으로

								  //(2) 남은부분 앞에서 쓰기
			memcpy(cpBufRear, pData + nTempSize, nSize - nTempSize);
			cpBufRear += (nSize - nTempSize);
		}

		dwDataSize += nSize;

		return true;
	}

	bool CircularQueue::Pop(char* pData, int nSize)
	{
		if (nSize > dwDataSize) //큐잉된 크기보다 큰경우 false
			return false;

		if (nSize <= (cpBufEnd - cpBufFront - 1))
		{
			//한번에 다 읽을수 있는경우
			memcpy(pData, cpBufFront, nSize);
			cpBufFront += nSize;
		}
		else
		{
			//(1) 버퍼끝까지 읽고      
			int nTempSize = cpBufEnd - cpBufFront;
			memcpy(pData, cpBufFront, nTempSize);
			cpBufFront = cRingBuf; //버퍼 제일 앞으로

								   //(2) 앞쪽부터 다시 읽어야 하는 경우
			memcpy(pData + nTempSize, cpBufFront, nSize - nTempSize);
			cpBufFront += (nSize - nTempSize);
		}

		dwDataSize -= nSize;

		return true;
	}

	long CircularQueue::GetQueueSize()
	{
		return RINGBUFSIZE + MAXPACKETSIZE - 1; //-1은 원형큐에서 Full Check를 위한 여분
	}

	long CircularQueue::GetRemainQueueSize()
	{
		return GetQueueSize() - dwDataSize;
	}
}