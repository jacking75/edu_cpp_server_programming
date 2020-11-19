#include "TcpSession.h"




namespace NServerNetLib
{	

	void TcpSession::Init(const int index)
	{
		mIndex = index;
		SocketFD = -1;
		SendSize = 0;

		pRecvBuffer = nullptr;
		pSendBuffer = nullptr;

		RemainingDataSize = 0;
		PrevReadPosInRecvBuffer = 0;
	}

	NET_ERROR_CODE TcpSession::SendSessionData(int maxClientSendBufferSize, const short packetId, const short bodySize, char* pMsg)
	{	  
		std::lock_guard<std::mutex> lock(sendPacketMutex);

		auto pos = SendSize;
		auto totalSize = (int16_t)(bodySize + PACKET_HEADER_SIZE);

		if ((pos + totalSize) > maxClientSendBufferSize)
		{
			return NET_ERROR_CODE::CLIENT_SEND_BUFFER_FULL;
		}

		PacketHeader pktHeader{ totalSize, packetId, (uint8_t)0 };
		memcpy(&pSendBuffer[pos], (char*)&pktHeader, PACKET_HEADER_SIZE);

		if (bodySize > 0)
		{
			memcpy(&pSendBuffer[pos + PACKET_HEADER_SIZE], pMsg, bodySize);
		}

		SendSize += totalSize;

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TcpSession::SendPacket(const SOCKET fd, const char* pMsg, const int size)
	{
		std::lock_guard<std::mutex> lock(sendPacketMutex);

		auto result = SendSocket(fd, pMsg, size);

		if (result.has_value() == false) {
			return NET_ERROR_CODE::SEND_SIZE_ZERO;
		}

		auto sendSize = result.value();
		if (sendSize < SendSize)
		{
			memmove(&pSendBuffer[0], &pSendBuffer[sendSize], SendSize - sendSize);
			SendSize -= sendSize;
		}
		else
		{
			SendSize = 0;
		}

		return NET_ERROR_CODE::NONE;
	}

	std::optional <int> TcpSession::SendSocket(const SOCKET fd, const char* pMsg, const int size)
	{
		if (size <= 0)
		{
			return std::nullopt;
		}

		auto result = send(fd, pMsg, size, 0);

		if (result <= 0)
		{
			return std::nullopt;
		}

		return result;
	}
	
}