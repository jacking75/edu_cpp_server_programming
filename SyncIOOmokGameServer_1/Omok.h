#pragma once

#include <vector>
#include <string>
#include <memory>
#include "ErrorCode.h"

namespace ChatServerLib
{
	class Omok {
    public:
        Omok();
        virtual ~Omok();

        void init(int userIndex);

        OmokPanPoint OmokPanPoints[19][19];
        NServerNetLib::ERROR_CODE GamePutStone(int userIndex, int xPos, int yPos);
        bool IsUserTurn(int userIndex);
	};

    class OmokPanPoint
    {
    public:
        enum PointType
        {
            None,
            Black = 1,
            White = 2,
        };
        PointType Type;

    };
}
