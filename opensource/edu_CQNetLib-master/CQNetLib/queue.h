#pragma once

#include "commonDef.h"
#include "spinLock.h"

namespace CQNetLib
{
	template < class TYPE >
	class ConcurrentQueue
	{
	public:
		ConcurrentQueue(INT32 nMaxSize = MAX_QUEUESIZE);
		~ConcurrentQueue(void);

		bool					PushQueue(TYPE typeQueueItem);		//queue에 데이타를 넣는다.
		void					PopQueue();								//queue에서 데이타를 지운다.

		bool					IsEmptyQueue();							//queue가 비었는지 알려준다.
		TYPE					GetFrontQueue();						//queue에서 데이타를 얻어온다.
		INT32 						GetQueueSize();							//queue에 현재 들어있는 item의 갯수를 알려준다.
		INT32 						GetQueueMaxSize() const { return m_nQueueMaxSize; }  //최대 queue사이즈를 알려준다
		void					SetQueueMaxSize(INT32 nMaxSize) { m_nQueueMaxSize = nMaxSize; }   //최대 queue size를 정해 준다
		void					ClearQueue();							//queue에 있는 모든 item을 삭제한다.


	private:
		SpinLock m_Lock;

		TYPE*					m_arrQueue = nullptr;
		INT32 						m_nQueueMaxSize = 0;						//큐에대한 사이즈 제한

		INT32 						m_nCurSize = 0;
		INT32 						m_nEndMark = 0;
		INT32 						m_nBeginMark = 0;

		SET_NO_COPYING(ConcurrentQueue);

	};

	template <class TYPE>
	ConcurrentQueue< TYPE >::ConcurrentQueue(INT32 nMaxSize)
	{
		m_arrQueue = new TYPE[nMaxSize];
		m_nQueueMaxSize = nMaxSize;
		ClearQueue();

	}

	template <class TYPE>
	ConcurrentQueue< TYPE >::~ConcurrentQueue(void)
	{
		delete[] m_arrQueue;
	}

	template <class TYPE>
	bool ConcurrentQueue< TYPE >::PushQueue(TYPE typeQueueItem)
	{
		SpinLockGuard lock(m_Lock);
		
		//정해놓은 사이즈를 넘었다면 더이상 큐에 넣지 않는다.
		if (m_nCurSize >= m_nQueueMaxSize)
			return false;

		m_nCurSize++;
		if (m_nEndMark == m_nQueueMaxSize)
			m_nEndMark = 0;

		m_arrQueue[m_nEndMark++] = typeQueueItem;

		return true;


	}

	template <class TYPE>
	TYPE ConcurrentQueue< TYPE >::GetFrontQueue()
	{
		SpinLockGuard lock(m_Lock);

		TYPE typeQueueItem;
		
		if (m_nCurSize <= 0)
			return nullptr;
		
		if (m_nBeginMark == m_nQueueMaxSize)
			m_nBeginMark = 0;
		
		typeQueueItem = m_arrQueue[m_nBeginMark++];

		return typeQueueItem;

	}

	template <class TYPE>
	void ConcurrentQueue< TYPE >::PopQueue()
	{
		SpinLockGuard lock(m_Lock);
		
		m_nCurSize--;
	}

	template <class TYPE>
	bool ConcurrentQueue< TYPE >::IsEmptyQueue()
	{
		SpinLockGuard lock(m_Lock);
		
		auto bFlag = (m_nCurSize > 0) ? true : false;		
		return bFlag;
	}

	template <class TYPE>
	INT32 ConcurrentQueue< TYPE >::GetQueueSize()
	{
		SpinLockGuard lock(m_Lock);

		auto nSize = m_nCurSize;
		return nSize;
	}

	template <class TYPE>
	void ConcurrentQueue< TYPE >::ClearQueue()
	{
		SpinLockGuard lock(m_Lock);
		
		m_nCurSize = 0;
		m_nEndMark = 0;
		m_nBeginMark = 0;
	}

}