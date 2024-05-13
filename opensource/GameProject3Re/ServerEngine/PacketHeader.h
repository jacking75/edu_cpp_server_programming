#pragma once

namespace hbServerEngine
{
#pragma  pack(push)
#pragma  pack(1)

#define CODE_VALUE 0x88

    struct PacketHeader
    {
        uint32_t   CheckCode;
        int32_t    nMsgID;
        int32_t    nSize;
        int32_t    nPacketNo; // 일련 번호 생성 = nMsgID^nSize+index(각 패킷마다 자동으로 인덱스가 증가); 일련 번호 복원 = pHeader->dwPacketNo - pHeader->nMsgID^pHeader->nSize;
        uint64_t   u64TargetID;
        uint32_t   dwUserData;    // 복제본에 있는 클라이언트의 경우 dwUserData가 copyguid 이며, 논리적 서비스의 경우 프로토콜의 일부가 클라이언트 연결 ID 역할을 한다
    };

    struct PacketHeader2
    {
        uint32_t   CheckCode : 8;
        uint32_t   dwSize : 24;
        uint32_t   dwPacketNo : 8;  //일련 번호 생성 = nMsgID^nSize+index(각 패킷마다 자동으로 인덱스가 증가); 일련 번호 복원 = pHeader->dwPacketNo - pHeader->nMsgID^pHeader->nSize;
        uint32_t   Compress : 1;
        uint32_t   Encrypt : 1;
        uint32_t   nMsgID : 22;
        uint64_t   u64TargetID;
        uint32_t   dwUserData;   // 복제본에 있는 클라이언트의 경우 dwUserData가 copyguid 이며, 논리적 서비스의 경우 프로토콜의 일부가 클라이언트 연결 ID 역할을 한다
    };


#pragma  pack(pop)

}

