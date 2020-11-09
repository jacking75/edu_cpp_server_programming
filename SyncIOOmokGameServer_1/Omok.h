#pragma once

#include "ErrorCode.h"
#include "OmokPanPoint.h"
#include <vector>

namespace ChatServerLib
{
	class Omok {
        const int OmokPanPointNumber = 19;

    public:
        Omok() = default;
        ~Omok() = default;

       void init();

       void printTest();

        ERROR_CODE CheckGameEnd(int xPos, int yPos);

        ERROR_CODE GamePutStone( int xPos, int yPos);

        void initType();

        bool IsBlackTurn = false;

        int OmokTurnUserIndex = -1;

        std::vector<std::vector<OmokPanPoint>> OmokPanPoints;
 
	};

}
