#include "NaveServer.h"
#include "NFConnection.h"
#include "NFConnectionManager.h"
//#include <Nave/NFLog.h>

namespace NaveNetLib {

	NFConnectionManager::NFConnectionManager(void) : m_iCount(0), m_iMaxCount(0)
	{
		m_vecConn.clear();
	}

	NFConnectionManager::~NFConnectionManager(void)
	{
		m_vecConn.clear();
	}

	void NFConnectionManager::Init(int iMaxCount)
	{
		std::lock_guard<std::mutex> Sync(m_Lock);

		m_iCount = 0;
		m_iMaxCount = iMaxCount;
		m_vecConn.reserve(iMaxCount);

		for(int i = 0; i < m_iMaxCount; ++i)
		{
			m_vecConn.push_back(nullptr);
		}
	}	

	void NFConnectionManager::SendPostAll( char* pPackte, int Len )		// 실제 Send 처리
	{
		for(int i = 0; i < m_iMaxCount; ++i)
		{
			if (!m_vecConn[i]) {
				continue;
			}

			m_vecConn[i]->SendPost(pPackte, Len);
		}
	}

	void NFConnectionManager::SendPostAll( NFPacket& Packet)
	{
		Packet.EncryptPacket();

		INT Len = Packet.m_Header.Size;
		CHAR* pPacket = (CHAR*)&Packet;

		SendPostAll(pPacket, Len);
	}


	/*void NFConnectionManager::Draw()
	{
		std::lock_guard<std::mutex> Sync(m_Lock);

		INT Count = 0;
		for(INT i = 0; i < m_iMaxCount; ++i)
		{
			if(m_vecConn[i])
				Count++;
		}

		LOG_ERROR((L"Leave 되지 않은 Count : %d", Count));
	}*/

	// Connect, Disconnect 를 관리하는 함수.
	void NFConnectionManager::Join(NFConnection* pUser)
	{
		std::lock_guard<std::mutex> Sync(m_Lock);

		if (!pUser) {
			return;
		}

		auto iIndex = pUser->GetIndex();
		
		if(m_vecConn[iIndex] == nullptr)
		{
			m_vecConn[iIndex] = pUser;
			++m_iCount;
		}
		else
		{
			//LOG_ERROR((L"[%04d] 중복 Join호출", iIndex));
		}
	}

	void NFConnectionManager::Levae(int iIndex)
	{
		std::lock_guard<std::mutex> Sync(m_Lock);

		if(m_vecConn[iIndex])
		{
			--m_iCount;
			m_vecConn[iIndex] = nullptr;
		}
		else
		{
			//LOG_ERROR((L"[%04d] 중복 Leave호출 Socket : %d", iIndex, WSAGetLastError()));
		}
	}

}