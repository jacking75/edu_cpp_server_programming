#pragma once


namespace COMMONLIB
{
#define RINGBUFSIZE 10
#define MAXPACKETSIZE 1024

	class CircularQueue
	{
	protected:
		char  cRingBuf[RINGBUFSIZE + MAXPACKETSIZE]; //수신링버퍼
		char  *cpBufFront, *cpBufRear; //수신링버퍼 관리변수
		char  *cpBufEnd; //버퍼의 끝위치
		long  dwDataSize; //큐잉된 데이터 크기

	public:
		CircularQueue();
		~CircularQueue();

	public:
		void Clear();
		bool IsBufferEmpty(); //비었는지 체크
		bool IsBufferFull(); //full 인지 체크
		bool Push(const char* pData, const int nSize);
		bool Pop(char* pData, int nSize);
		long GetQueueSize();  //큐 전체크기
		long GetRemainQueueSize(); //큐에 넣을수 있는 남은 크기

		long GetDataSize() { return dwDataSize; }
	};
}