/** 
 *  @file		NFPacket.h
 *  @brief		패킷 클래스
 *  @remarks	
 *  @author		강동명(edith2580@gmail.com)
 *  @date		2009-04-02
 */

/*
2017. 05.25 최흥배 수정
*/

#pragma once

#ifdef _WIN32
	#pragma pack( push ,1 )
#else    
	#pragma pack(1)
#endif

namespace NaveClientNetLib {

	using USHORT = unsigned short;
	using UINT = unsigned int;

	const int		DEF_BUFSIZE = 1024;						/// Mem Pool and Socket Buffer size
	const int		DEF_PACKETSIZE = 1016;						/// Packet Size 
	const int		DEF_MAXPACKETSIZE = 1024;						/// Max Packet Size ( Header + Packet )

	const char		CONNECT_CHECK_DATA[] = "NFG";						/// CheckSum 패킷 (처음으로 Server에 Connect 한후 전달된다)
	const int		CONNECT_CHECK_SIZE =	4;							/// 초기 접속 오는 패킷 사이즈 ** WARINING **

	/// 패킷 헤더 구조체
	typedef struct PACKETHEADER
	{
		USHORT	Size;		/// 실제 패킷 크기
		USHORT	Command;	/// 패킷의 종류
		UINT	CheckSum;	/// 체크섬
	}*LPPACKETHEADER;

	const int HEADERSIZE = 8;			/// 패킷 헤더의 사이즈

	/** 
	 *  @class        NFPacket
	 *  @brief        한개의 패킷을 관리 하기 위한 클래스 객체
	 *  @remarks     패킷의 구조										\r\n
																		\r\n
					Header( Size, Command) + CheckSum + Data			\r\n
					------------------------------------------------------------0-------------------\r\n
					| 2byte | 2byte | CheckSum(4Byte) | Header.Size-Header 만큼의 실제 Packet 용량 |\r\n
					--------------------------------------------------------------00----------------\r\n
																		\r\n
					예)													\r\n
					#define USER_LOGIN	1								\r\n
																		\r\n
					typedef struct stLogin								\r\n
					{													\r\n
						CHAR	UserID[16];								\r\n
						CHAR	UserPass[12];							\r\n
					}LOGIN, *LPLOGIN;									\r\n
																		\r\n
					// Header 정보										\r\n
					Packet.m_Header.Size = sizeof(LOGIN);				\r\n
					Packet.m_Header.Command = USER_LOGIN;				\r\n
																		\r\n
					// 사용법											\r\n
					static LPLOGIN login = (LPLOGIN)Packet.m_Packet;	\r\n
					login->UserID;										\r\n
					login->UserPass;									\r\n

	 *                
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-05
	 */
	class NFPacket 
	{
	public:
		/// NFPacket 생성자
		NFPacket();
		
		/// NFPacket 소멸자
		~NFPacket() = default;

		/// 패킷 초기화
		void Init();

		/**
		 * @brief	헤더의 Command 얻기
		 * @return	Command 정보
		 */
		inline USHORT GetCommand() const 
		{
			return m_Header.Command;
		}

		/**
		 * @brief	헤더에 Command를 지정한다.
		 * @param shCommand Command 값
		 */
		inline void SetCommand(USHORT shCommand)
		{
			m_Header.Command = shCommand;
		}

		/**
		 * @brief	패킷 사이즈 구하기
		 * @return  패킷 사이즈
		 */
		inline int GetSize()
		{
			return m_Header.Size-HEADERSIZE;
		}

		/**
		 * @brief	패킷의 사이즈를 지정한다.
		 * @param shDataSize 
		 */
		inline void SetSize(USHORT shDataSize)
		{
			// Size = Header + DataSize;
			m_Header.Size = HEADERSIZE + shDataSize;
		}	

		/// 패킷을 암호화한다.
		UINT EncryptPacket(const bool isEncrypt=false);

		/// 패킷을 디코딩한다.
		void DecryptPacket();

		/// CheckSum 변수와 새로 생성한 체크섬이 동일한지 검사한다.
		bool IsAliveChecksum(const bool isEncrypt = false);


	public:
		/// 패킷의 헤더 정보
		PACKETHEADER		m_Header;

		/// 실제 패킷의 정보
		char				m_Packet[DEF_PACKETSIZE];
					
	};

}

#pragma pack()
