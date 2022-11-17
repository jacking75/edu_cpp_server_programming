/** 
 *  @file		NFIOBuffer.h
 *  @brief		I/O Buffering, Packet I/O Buffering
 *  @remarks	
 *  @author		강동명(edith2580@gmail.com)
 *  @date		2009-04-02
 */

#pragma once

namespace NaveNetLib { 

	class NFPacket;

	/**
	 @class		
	 @author	강동명(edith2580@gmail.com)
	 @date		2009-03-03
	 @brief		
	 @warning   
	*/
	/** 
	 *  @class		NFIOBuffer
	 *  @brief		I/O Buffering 클래스
	 *  @remarks	I/O Buffering 을 위한 클래스. Sock을 이용해 패킷을 주고 받을때 패킷이				\r\n
	 *  			합쳐오거나 분할되어 올때 그 패킷을 관리 및 처리할때 사용된다.(IOBuffer을			\r\n
	 *  			상속받은 PacektIOBuffer 클래스로 처리하게..											\r\n
	 *  																								\r\n
	 *  			기본적으로 PacketBuffer에 저장되는 스트링(?)의 형식은 다음과 같다.					\r\n
	 *  			---------------------------------------------------------------------------------------	\r\n
	 *  			| 2byte | 2byte | CheckSum(4Byte) | Header.Siz-Headere 만큼의 실제 Packet 용량 Header |	\r\n
	 *  			---------------------------------------------------------------------------------------	\r\n
	 *  			IOBuffer클래스는 위의 패킷을 하나의 배열에 순차적으로 넣어 그 패킷을				\r\n
	 *  			관리한다.  2Byte는 65535까지의 길이를 처리한다.										\r\n
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-04
	 */
	class NFIOBuffer
	{
	public:
		/// NFIOBuffer 생성자
		NFIOBuffer();
		/// NFIOBuffer 소멸자
		virtual ~NFIOBuffer();

	public:
		/// 사용된 버퍼의 크기 계산.
		void	CalcBuffered();

		/**
		 * @brief	버퍼 포인터를 얻어옵니다.
		 * @return  버퍼 포인터
		 */
		inline char* GetBuffer() { return	m_cBuf; }
		/**
		 * @brief	Head Pos를 설정합니다.
		 * @param Head Head Pos값
		 */
		inline void SetHead(int Head) { m_iHead = Head; CalcBuffered(); }
		/**
		 * @brief	Head Pos를 가져옵니다.
		 * @return	Head Pos 값
		 */
		inline int	GetHead() { return m_iHead; }
		/**
		 * @brief	Tail Pos를 설정합니다.
		 * @param Tail Tail Pos값
		 */
		inline void SetTail(int Tail) { m_iTail = Tail; }
		/**
		 * @brief	Tail Pos를 가져옵니다.
		 * @return	Tail Pos 값
		 */
		inline int	GetTail() { return m_iTail; }
		/**
		 * @brief	버퍼의 전체 크기를 얻어옵니다.
		 * @return  버퍼의 전체 크기
		 */
		inline int	GetBufSize() { return m_iBufSize; }

		/**
		 * @brief	사용중인 버퍼의 길이를 얻어옵니다.
		 * @return	사용중인 버퍼의 길이
		 */
		inline int GetBufferUsed() { return m_iBuffered; }
		/**
		 * @brief	비어있는 버퍼의 길이를 얻어옵니다.
		 * @return	비어있는 버퍼의 길이
		 */
		inline int GetEmptyBuffer() { return m_iBufSize - m_iBuffered; }

		/**
		 * @brief	버퍼를 초기화합니다.
		 */
		void	InitIOBuf();

		/**
		 * @brief	버퍼를 추가합니다.
		 * @param Buffer	버퍼
		 * @param Size		크기
		 * @return	추가한 길이
		 */
		int		Append(const char* Buffer, int Size);

		/**
		 * @brief	버퍼를 읽습니다.
		 * @param Buffer	버퍼
		 * @param Size		읽을 크기
		 * @return	읽어들인 길이
		 */
		int		GetData(char* Buffer, int Size);

		/**
		 * @brief	버퍼길이 체크합니다.
		 * @param Size	체크할 길이
		 * @return	체크한 길이 성공시 Size값과 동일
		 */
		int		CheckData(int Size);
		
		/**
		 * @brief	버퍼를 생성합니다.
		 * @param BufSize 생성할 버퍼 크기
		 */
		void	NewIOBuf(int BufSize);

		/**
		 * @brief	버퍼를 삭제합니다.
		 */
		void	DeleteIOBuf();

	private:
		/// 읽을 버퍼의 시작위치.
		int		m_iHead;
		/// 사용된 버퍼의 끝.
		int		m_iTail;
		/// 생성한 버퍼의 사이즈
		int		m_iBufSize;
		/// 생성한 버퍼의 포인터.
		char*	m_cBuf;

		/// 사용된 버퍼의 사이즈 이값이 음수면 Over Flow..
		int		m_iBuffered;
	};

	/** 
	 *  @class        NFPacketIOBuffer
	 *  @brief        Packet I/O Buffering 클래스
	 *  @remarks      NFPacket에 자동화 되어있는 클래스		\r\n
	 *	   			  서버와 클라이언트간 통신에서 사용된다.
	 *  @par          
	 *  @author  Edith
	 *  @date    2009-04-04
	 */
	class NFPacketIOBuffer : public NFIOBuffer
	{
	public:
		/// NFPacketIOBuffer 생성자
		NFPacketIOBuffer();
		/// NFPacketIOBuffer 소멸자
		virtual ~NFPacketIOBuffer();

	public:
		/// 버퍼를 Lock 합니다.
		void	Lock();

		/// 버퍼를 UnLock 합니다.
		void	UnLock();

		// 한개분량의 패킷을 얻어온다.
		int		GetPacket(NFPacket* Packet);

		// 패킷체크
		bool	CheckPacket();

	private:
		/// Lock 과 UnLock에서 사용할 변수 
		int		m_iLockHead;
	};


}