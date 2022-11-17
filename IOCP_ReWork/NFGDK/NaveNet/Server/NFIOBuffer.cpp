#include <memory.h>
#include "NFIOBuffer.h"
#include "NFPacket.h"

namespace NaveNetLib { 

	// IOBuffer의 기본크기 패킷 1024개만큼 처리가능
	const int IOBUF_DEF_SIZE = DEF_MAXPACKETSIZE*128;

	NFIOBuffer::NFIOBuffer() : m_iHead(0), m_iTail(0), m_iBufSize(0), m_cBuf(0), m_iBuffered(0)
	{
	}

	NFIOBuffer::~NFIOBuffer()
	{
		DeleteIOBuf();
	}

	void NFIOBuffer::InitIOBuf()
	{
		m_iHead = m_iTail = 0;
		m_iBuffered = 0;

		if(m_cBuf)
			memset(m_cBuf, 0, sizeof(m_cBuf));
	}

	void NFIOBuffer::NewIOBuf(int BufSize)
	{
		if (BufSize <= 0) {
			BufSize = IOBUF_DEF_SIZE;
		}

		m_cBuf = new char[BufSize];
		if(m_cBuf == NULL)
		{
	//		throw "NFIOBuffer::NewIOBuf : Memory allocation failure!";
			return;
		}
		
		m_iBufSize = BufSize;
		
		InitIOBuf();
	}

	void NFIOBuffer::DeleteIOBuf()
	{
		if(m_cBuf)
		{
			delete []m_cBuf;
			m_cBuf = NULL;
		}

		m_iBufSize = 0;
		m_iHead = m_iTail = 0;
		m_iBuffered = 0;
	}

	// Size 만큼 Buffer의 내용을 쓴다.
	int NFIOBuffer::Append(const char* Buffer, int Size)
	{
		// 오버플로우가 된다. 
		if(m_iBuffered + Size >= m_iBufSize)
		{
	//		throw "NFIOBuffer::Append : Buffer overflow";
			return -1;
		}

		auto aSize = 0;
		auto Added = 0;

		// 모든 Size를 추가할때까지 처리한다.
		while(Size > 0)
		{
			if (Size > m_iBufSize - m_iTail) {
				aSize = m_iBufSize - m_iTail;
			}
			else {
				aSize = Size;
			}

			if(aSize)
			{
				memcpy(m_cBuf+m_iTail, Buffer, aSize);

				Added += aSize;
				Size -= aSize;
				Buffer += aSize;
				m_iTail += aSize;
				
				if (m_iTail >= m_iBufSize) {
					m_iTail -= m_iBufSize;
				}
			}
		}

		CalcBuffered();

		return Added;
	}

	// Size만큼 데이타를 읽어 Buffer에 쓴다.
	int	NFIOBuffer::GetData(char* Buffer, int Size)
	{
		// 써있는 데이타 보다 많이 읽으면 써있는 데이타만 읽게 한다.
		if (GetBufferUsed() < Size) {
			Size = GetBufferUsed();
		}

		if (Size <= 0) {
			return 0;
		}

		// 잘려있으면. 처리한다.
		if(m_iHead+Size >= m_iBufSize)
		{
			// 여기에 지금 버그가 있다.
			auto Size1 = m_iBufSize - m_iHead;
			memcpy(Buffer, m_cBuf+m_iHead, Size1);
			memcpy(Buffer+Size1, m_cBuf, Size-Size1);
		}
		else	// 안잘려 있으면.
		{
			memcpy(Buffer, m_cBuf+m_iHead, Size);
		}

		m_iHead += Size;
		if (m_iHead >= m_iBufSize) {
			m_iHead -= m_iBufSize;
		}

		CalcBuffered();
		return Size;
	}

	int NFIOBuffer::CheckData(int Size)
	{
		// 써있는 데이타 보다 많이 읽으면 써있는 데이타만 읽게 한다.
		if (GetBufferUsed() < Size) {
			Size = GetBufferUsed();
		}

		if (Size <= 0) {
			return 0;
		}

		m_iHead += Size;
		if (m_iHead >= m_iBufSize) {
			m_iHead -= m_iBufSize;
		}

		CalcBuffered();
		return Size;
	}


	void NFIOBuffer::CalcBuffered()
	{
		if (m_iHead > m_iTail) {
			m_iBuffered = m_iBufSize - m_iHead + m_iTail;
		}
		else {
			m_iBuffered = m_iTail - m_iHead;
		}
	}


	NFPacketIOBuffer::NFPacketIOBuffer() : m_iLockHead(0)
	{
		// 초기화.
		NFIOBuffer::NFIOBuffer();
	}

	NFPacketIOBuffer::~NFPacketIOBuffer()
	{
		DeleteIOBuf();
	}

	void NFPacketIOBuffer::Lock()
	{
		m_iLockHead = GetHead();
	}

	void NFPacketIOBuffer::UnLock()
	{
		SetHead(m_iLockHead);
	}

	bool NFPacketIOBuffer::CheckPacket()
	{
		PACKETHEADER	header;
		auto HeaderSize = (int)sizeof(PACKETHEADER);

		// Header
		if(GetBufferUsed() < HeaderSize)
		{
			// 에러가 아니라 데이타가 없는거다.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < P_SIZE";
			return false;
		}

		if(HeaderSize != GetData((char*)&header, HeaderSize))
		{
			// 헤더를 원래대로 초기화 한다.
	//		throw "NFPacketIOBuffer::GetPacket : Packet Header Size != P_SIZE";
			return false;
		}

		int PacketSize = header.Size-HeaderSize;
		// 헤더는 제대로 들어갔는데 버퍼가 제대로 들어갔는지 확인한다.
		if(GetBufferUsed() < PacketSize)
		{
			// 헤더를 원래대로 초기화 한다.
			// 에러가 아니라 데이타가 없는거다.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < PacketLen";
			return false;
		}

		
		int GetSize = CheckData(PacketSize);
		
		if(GetSize != PacketSize)
		{
			// 헤더를 원래대로 초기화 한다.
	//		throw "NFPacketIOBuffer::GetPacket : Packet Data Read Faild, PacketSize != GetSize";
			return false;
		}

		return true;
	}

	int NFPacketIOBuffer::GetPacket(NFPacket* Packet)
	{
		int OldHead = GetHead();

		int HeaderSize = HEADERSIZE;

		// Header
		if(GetBufferUsed() < HeaderSize)
		{
			// 에러가 아니라 데이타가 없는거다.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < P_SIZE";
			return -1;
		}

		if(HeaderSize != GetData((char*)&Packet->m_Header, HeaderSize))
		{
			// 헤더를 원래대로 초기화 한다.
			SetHead(OldHead);
	//		throw "NFPacketIOBuffer::GetPacket : Packet Header Size != P_SIZE";
			return -2;
		}

		int PacketSize = Packet->m_Header.Size-HeaderSize;
		// 헤더는 제대로 들어갔는데 버퍼가 제대로 들어갔는지 확인한다.
		if(GetBufferUsed() < PacketSize)
		{
			// 헤더를 원래대로 초기화 한다.
			SetHead(OldHead);
			// 에러가 아니라 데이타가 없는거다.
	//		throw "NFPacketIOBuffer::GetPacket : IOBuffer Used Memory < PacketLen";
			return -3;
		}

		// 아직 데이타가 제대로 처리안됐다.
		int GetSize = GetData(Packet->m_Packet, PacketSize);
		if(GetSize != PacketSize)
		{
			// 헤더를 원래대로 초기화 한다.
			SetHead(OldHead);
	//		throw "NFPacketIOBuffer::GetPacket : Packet Data Read Faild, PacketSize != GetSize";
			return -4;
		}

		// 패킷을 제대로 읽어들였다.
		return 1;
	}

}