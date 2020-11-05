#pragma once

#include "ErrorCode.h"
#include <memory>
#include "OmokPanPoint.h"
#include <string>

namespace ChatServerLib
{
	class Omok {
    public:
        Omok();
        virtual ~Omok();

       void init();

        NServerNetLib::ERROR_CODE CheckGameEnd(int xPos, int yPos);
        NServerNetLib::ERROR_CODE GamePutStone(int userIndex, int xPos, int yPos);
        bool IsUserTurn(int userIndex);

        bool IsBlackTurn = false;
        int OmokTurnUserIndex = -1;

        OmokPanPoint** OmokPanPoints;
	};

}
