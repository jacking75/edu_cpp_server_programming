/** 
 *  @file  		NFServerLink.h
 *  @brief 		Server To Server 용 Connect 객체
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

namespace NaveNetLib {

	// NFServerLink는 Server가 NFServerCtrl로 Connection을 하기위해 제작된 구조체이다
	// NFServerLink을 선언후 Create로 Server에 Connection하게 한다.

	class NFServerLink : public NFConnection
	{
	public:
		NFServerLink(void) = default;
		virtual ~NFServerLink(void) = default;

	public:
		virtual void		Clear() { return; };

		virtual bool		Close_Open(bool bForce=false );

		// 이 객체 생성 
		virtual bool		Create(	DWORD			dwIndex,
									HANDLE			hIOCP,
									NFPacketPool*	pPacketPool,
									wchar_t*			sIP,
									int				nPort,
									int				nMaxBuf);

		virtual bool		SendOpenPacket() { return true; };

	protected:
		// Socket과 리스너 연결 및 스탠바이 상태 만듬  
		virtual bool		Open();

		// 내부 처리 전송 함수 
		virtual bool			DispatchPacket( LPOVERLAPPEDPLUS lpOverlapPlus, NFUpdateManager* pUpdateManager);

		// 실제 패킷을 처리하는 부분이다.
		virtual void		DispatchPacket( NFPacket& Packet ) 
		{
			// Nave::Sync::CSSync Live(&m_Sync);
		};

	protected:
		wchar_t	m_strIP[32];
		int		m_iPort;
	};

}