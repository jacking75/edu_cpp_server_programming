#pragma once

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message.h>

using namespace google;

#pragma pack(push, 1) 
namespace packetdef
{
	typedef enum PacketRegion_ : unsigned short
	{
		PACKET_REG_LOGIN = 0x0000
	} PacketRegion;


	typedef enum PacketID_: unsigned short 
	{
		EnterWorld = PACKET_REG_LOGIN + 0x0000,
		EnterPacket,
		Move,
		LeaveWorld,
		LeavePacket,
		LoginReq,
		LoginAck,
		CharSelectReq,
		CharSelectAck,
		ZoneMoveReq,
		ZoneMoveAck,
		PingReq,
		Invalid = 10000,


	} PacketID;

	struct PacketHeader
	{
		protobuf::uint16 dataSize;
		PacketID id;
		protobuf::uint64 vsession;
		protobuf::uint16 Seq;
		protobuf::uint16 Reserv;
		
		PacketHeader()
		{
		}

		PacketHeader(PacketID id_, protobuf::uint32 size_) :
			dataSize(size_),
			id(id_),
			vsession(0),
			Seq(0),
			Reserv(0)
		{
		}
	};

	const int HeaderSize = sizeof(PacketHeader);
}
#pragma pack(pop)


