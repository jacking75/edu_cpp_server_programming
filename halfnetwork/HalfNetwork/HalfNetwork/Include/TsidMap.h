#ifndef __tsidmap_h__
#define __tsidmap_h__

#pragma once

#include "TsQueue.h"
#include "ace/Map_Manager.h"

namespace HalfNetwork
{

	////////////////////////////
	// Description:
	//   Register / UnRegister ID
	////////////////////////////
	template<uint32 t_max>
	class IDQueueT
	{
	public:
		//////////////////////////////////////
		// Description:
		//   put ID in the queue
		//////////////////////////////////////
		IDQueueT(uint32 offset) : m_offset(offset)
		{
			for (uint32 i = 0; i < t_max; ++i)
				m_idleIdQueue.Push(i + m_offset);
		}

		/////////////////////
		// Description:
		//   Issue an ID
		////////////////////
		inline uint32 Pop()
		{
			uint32 id = Invalid_ID;
			if (false == m_idleIdQueue.Pop(id))
				return Invalid_ID;
			return id;
		}

		///////////////////////
		// Description:
		//   Restore a ID
		///////////////////////
		inline void Push(uint32 id)
		{
			if (IsValidId(id))
			{
				m_idleIdQueue.Push(id);
			}
			else
			{
				//assert(false);
			}
		}

		inline bool IsValidId(uint32 id) const
		{
			return (Invalid_ID != id) && (id >= m_offset) && (id < (t_max + m_offset));
		}

	private:
		uint32				m_offset;
		TSQueueT<uint32>	m_idleIdQueue;
	};

	////////////////////////////////////////////////
	// Description:
	//   Thread safe ID Map
	//   Register / UnRegister an ID using IDQueueT
	////////////////////////////////////////////////
	template<class T, uint32 t_max>
	class TsIDMapT
	{
	public:
		typedef ACE_Map_Manager<uint32, T*, ACE_Thread_Mutex> TMap;

	public:
		TsIDMapT() : m_idleIdQueue(0)
		{}

		~TsIDMapT()
		{}

		uint32 Register(T* p)
		{
			uint32 id = m_idleIdQueue.Pop();
			if (id == Invalid_ID)
				return Invalid_ID;
			m_activeIdMap.bind(id, p);
			return id;
		}

		void UnRegister(uint32 id)
		{
			T* t = NULL;
			if (-1 != m_activeIdMap.find(id, t))
			{
				m_activeIdMap.unbind(id);
				m_idleIdQueue.Push(id);
			}
		}

		T* Get(uint32 id)
		{
			T* t = NULL;
			if (-1 == m_activeIdMap.find(id, t))
				return NULL;
			return t;
		}

		uint32 Size()
		{
			return (uint32)m_activeIdMap.current_size();
		}

	private:
		TMap			m_activeIdMap;
		IDQueueT<t_max> m_idleIdQueue;
	};

	///////////////////////////////////////////
	// Description:
	//   Thread safe Map
	///////////////////////////////////////////
	template<class T, uint32 t_max>
	class TsMapT
	{
	public:
		typedef ACE_Map_Manager<uint32, T*, ACE_Thread_Mutex> TMap;

	public:
		uint32 Register(T* p, uint32 id)
		{
			m_container.bind(id, p);
			return id;
		}

		void UnRegister(uint32 id)
		{
			T* t = NULL;
			if (-1 != m_container.find(id, t))
			{
				m_container.unbind(id);
			}
		}

		T* Get(uint32 id)
		{
			T* t = NULL;
			if (-1 == m_container.find(id, t))
				return NULL;
			return t;
		}

		uint32 Size()
		{
			return m_container.current_size();
		}

	private:
		TMap	m_container;
	};

} // namespace HalfNetwork

#endif // __tsidmap_h__