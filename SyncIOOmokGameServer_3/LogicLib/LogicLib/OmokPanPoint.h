#pragma once

namespace ChatServerLib
{
    class OmokPanPoint
    {
    public:
        enum class PointType
        {
            None,
            Black = 1,
            White = 2,
        };
        PointType Type;

    };
}