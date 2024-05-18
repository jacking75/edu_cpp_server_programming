#pragma once

#include "commonDef.h"
#include "spinLock.h"

namespace CQNetLib
{

	class RingBuffer
	{
	public:
		RingBuffer(void)
		{
		}

		virtual ~RingBuffer(void)
		{
			if (m_pBeginMark != nullptr)
			{
				delete[] m_pBeginMark;
			}
		}

		//링버퍼 메모리 할당
		bool Create(const INT32 nBufferSize = MAX_RINGBUFSIZE)
		{
			SAFE_DELETE_ARRAY(m_pBeginMark);

			m_pBeginMark = new char[nBufferSize];

			if (m_pBeginMark == nullptr)
			{
				return false;
			}

			m_pEndMark = m_pBeginMark + nBufferSize - 1;
			m_nBufferSize = nBufferSize;

			Initialize();
			return true;
		}
		
		//초기화
		bool Initialize()
		{
			SpinLockGuard lock(m_Lock);

			m_nUsedBufferSize = 0;
			m_pCurrentMark = m_pBeginMark;
			m_pGettedBufferMark = m_pBeginMark;
			m_pLastMoveMark = m_pEndMark;
			m_uiAllUserBufSize = 0;

			return true;
		}

		//할당된 버퍼 크기를 반환한다.
		inline INT32 		GetBufferSize() { return m_nBufferSize; }

		//해당하는 버퍼의 포인터를 반환
		inline char*	GetBeginMark() { return m_pBeginMark; }
		inline char*	GetCurrentMark() { return m_pCurrentMark; }
		inline char*	GetEndMark() { return m_pEndMark; }

		char* ForwardMark(INT32 nForwardLength)
		{
			SpinLockGuard lock(m_Lock);

			char* pPreCurrentMark = nullptr;

			//링버퍼 오버플로 체크
			if (m_nUsedBufferSize + nForwardLength > m_nBufferSize)
			{
				return nullptr;
			}

			if ((m_pEndMark - m_pCurrentMark) >= nForwardLength)
			{
				pPreCurrentMark = m_pCurrentMark;
				m_pCurrentMark += nForwardLength;
			}
			else
			{
				//순환 되기 전 마지막 좌표를 저장
				m_pLastMoveMark = m_pCurrentMark;
				m_pCurrentMark = m_pBeginMark + nForwardLength;
				pPreCurrentMark = m_pBeginMark;
			}

			return pPreCurrentMark;
		}

		char* ForwardMark(INT32 nForwardLength, INT32 nNextLength, UINT32 dwRemainLength)
		{
			SpinLockGuard lock(m_Lock);

			//링버퍼 오버플로 체크
			if (m_nUsedBufferSize + nForwardLength + nNextLength > m_nBufferSize)
			{
				return nullptr;
			}

			if ((m_pEndMark - m_pCurrentMark) > (nNextLength + nForwardLength))
			{
				m_pCurrentMark += nForwardLength;
			}			
			else //남은 버퍼의 길이보다 앞으로 받을 메세지양 크면 
			{
				//현재 메세지를 처음으로 복사한 다음 순환 된다.

				//순환 되기 전 마지막 좌표를 저장
				m_pLastMoveMark = m_pCurrentMark;
				CopyMemory(m_pBeginMark, m_pCurrentMark - (dwRemainLength - nForwardLength), dwRemainLength);
				m_pCurrentMark = m_pBeginMark + dwRemainLength;
			}

			return m_pCurrentMark;
		}

		void BackwardMark(const char* pBackwardMark)
		{
			SpinLockGuard lock(m_Lock);
			m_pCurrentMark = const_cast<char*>(pBackwardMark);
		}

		//사용된 버퍼 해제
		void ReleaseBuffer(INT32 nReleaseSize)
		{
			SpinLockGuard lock(m_Lock);
			m_nUsedBufferSize -= nReleaseSize;
		}

		//사용된 버퍼 크기 반환
		INT32 GetUsedBufferSize() { return m_nUsedBufferSize; }
		
		//사용된 버퍼양 설정(이것을 하는 이유는 SendPost()함수가 멀티 쓰레드에서 돌아가기때문에
		//PrepareSendPacket()에서(ForwardMark()에서) 사용된 양을 늘려버리면 PrepareSendPacket한다음에 데이터를
		//채워 넣기전에 바로 다른 쓰레드에서 SendPost()가 불린다면 엉뚱한 쓰레기 데이터가 갈 수 있다.
		//그걸 방지하기 위해 데이터를 다 채운 상태에서만 사용된 버퍼 사이즈를 설정할 수 있어야한다.
		//이함수는 sendpost함수에서 불리게 된다.
		void SetUsedBufferSize(INT32 nUsedBufferSize)
		{
			SpinLockGuard lock(m_Lock);

			m_nUsedBufferSize += nUsedBufferSize;
			m_uiAllUserBufSize += nUsedBufferSize;
		}

		//누적 버퍼 사용양 반환
		INT32 GetAllUsedBufferSize() { return m_uiAllUserBufSize; }


		//버퍼 데이터 읽어서 반환
		char* GetBuffer(INT32 nReadSize, INT32& refReadSize)
		{
			SpinLockGuard lock(m_Lock);

			char* pRet = nullptr;

			//마지막까지 다 읽었다면 그 읽어들일 버퍼의 포인터는 맨앞으로 옮긴다.
			if (m_pLastMoveMark == m_pGettedBufferMark)
			{
				m_pGettedBufferMark = m_pBeginMark;
				m_pLastMoveMark = m_pEndMark;
			}

			//현재 버퍼에 있는 size가 읽어들일 size보다 크다면
			if (m_nUsedBufferSize > nReadSize)
			{
				//링버퍼의 끝인지 판단.
				if ((m_pLastMoveMark - m_pGettedBufferMark) >= nReadSize)
				{
					refReadSize = nReadSize;
					pRet = m_pGettedBufferMark;
					m_pGettedBufferMark += nReadSize;
				}
				else
				{
					refReadSize = (INT32)(m_pLastMoveMark - m_pGettedBufferMark);
					pRet = m_pGettedBufferMark;
					m_pGettedBufferMark += refReadSize;
				}
			}
			else if (m_nUsedBufferSize > 0)
			{
				//링버퍼의 끝인지 판단.
				if ((m_pLastMoveMark - m_pGettedBufferMark) >= m_nUsedBufferSize)
				{
					refReadSize = m_nUsedBufferSize;
					pRet = m_pGettedBufferMark;
					m_pGettedBufferMark += m_nUsedBufferSize;
				}
				else
				{
					refReadSize = (INT32)(m_pLastMoveMark - m_pGettedBufferMark);
					pRet = m_pGettedBufferMark;
					m_pGettedBufferMark += refReadSize;
				}
			}

			return pRet;
		}


	private:
		SpinLock m_Lock;

		char*			m_pBeginMark = nullptr;			//버퍼의 처음부분을 가리키고 있는 포인터
		char*			m_pEndMark = nullptr;				//버퍼의 마지막부분을 가리키고 있는 포인터
		char*			m_pCurrentMark = nullptr;			//버퍼의 현재 부분을 가리키고 있는 포인터
		char*			m_pGettedBufferMark = nullptr;	//현재까지 데이터를 읽은 버퍼의 포인터
		char*			m_pLastMoveMark = nullptr;		//ForwardMark되기 전에 마지막 포인터

		INT32 				m_nBufferSize = 0;		//버퍼의 총 크기
		INT32 			m_nUsedBufferSize = 0;	//현재 사용된 버퍼의 크기
		UINT32			m_uiAllUserBufSize = 0; //총 처리된 데이터량


	private:
		// No copies do not implement
		RingBuffer(const RingBuffer &rhs);
		RingBuffer &operator=(const RingBuffer &rhs);
	};

}