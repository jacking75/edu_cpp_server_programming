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
        ~Omok();

       void init();

        NServerNetLib::ERROR_CODE CheckGameEnd(int xPos, int yPos);
        NServerNetLib::ERROR_CODE GamePutStone( int xPos, int yPos);
        void initType();

        bool IsBlackTurn = false;
        int OmokTurnUserIndex = -1;

        OmokPanPoint** OmokPanPoints;
	};

}
