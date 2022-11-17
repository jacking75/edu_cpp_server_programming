/** 
 *  @file  		NFConnectionManager.h
 *  @brief 		Connection을 관리하는 관리객체
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

//#include <Nave/NFSync.h>
#include <mutex>
#include <vector>

#include "NFPacket.h"


namespace NaveNetLib {

	class NFConnection;

	/** 
	 *  @class        NFConnectionManager
	 *  @brief        특정한 커넥션 리스트를 관리하기 위한 메니져 객체 동시에 여러개가 생길수 있다.
	 *  @remarks      
	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-08-28
	 */
	class NFConnectionManager
	{
	public:
		NFConnectionManager(void);
		~NFConnectionManager(void);

	public:
		/**
		 * @brief	메니져 초기화
		 * @param iMaxCount 관리 가능한 최대 커넥션 개수
		 */
		void Init(int iMaxCount);

		/**
		 * @brief	유저를 등록시킨다.
		 * @param pUser 유저의 연결객체 포인터
		 */
		void Join(NFConnection* pUser);

		/**
		 * @brief	해당 인덱스의 객체를 삭제한다.
		 * @param iIndex 삭제할 인덱스 
		 */
		void Levae(int iIndex);
		
		/**
		 * @brief	현재 등록된 연결객체 개수 얻기
		 * @return  커넥션 개수
		 */
		int	GetCount() { return m_iCount; }

		/**
		 * @brief	관리하는 연결객체 최대 개수
		 * @return  최대 연결객체 개수
		 */
		int GetMaxCount() { return m_iMaxCount; }

		/**
		 * @brief	등록된 모든 연결객체에 패킷을 전달한다.
		 * @param pPackte 보낼 패킷 버퍼
		 * @param Len 보낼 패킷 사이즈
		 */
		virtual void		SendPostAll( char* pPackte, int Len );

		/**
		 * @brief	등록된 모든 연결객체에 패킷을 전달한다.
		 * @param Packet 보낼 패킷 구조체
		 */
		virtual void		SendPostAll( NFPacket&	Packet);

		//void Draw();

	private:
		/// 연결객체 관리 변수
		std::vector<NFConnection*>						m_vecConn;
		typedef std::vector<NFConnection*>::iterator	iterConn;

		/// 현재 등록된 연결객체의 개수
		int												m_iCount;
		/// 관리가 가능한 최대 연결객체의 개수
		int												m_iMaxCount;

		/// Sync 객체
		std::mutex									m_Lock;
	};

}