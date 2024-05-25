#pragma once

namespace hbServerEngine
{
#pragma  pack(push)
#pragma  pack(1)

#define CODE_VALUE 0x88

    struct PacketHeader
    {
        uint16_t    MsgID;
        uint16_t    TotalSize;
        char        Attribute;
    };
      
#pragma  pack(pop)

}

