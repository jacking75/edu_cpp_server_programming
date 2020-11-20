#pragma once

#include "ErrorCode.h"
#include "OmokPanPoint.h"
#include <vector>

namespace ChatServerLib
{
	class Omok {
     

    public:
        Omok() = default;
        ~Omok() = default;

       void init();

       void printTest();

        ERROR_CODE CheckGameEnd(int xPos, int yPos);

        ERROR_CODE GamePutStone( int xPos, int yPos);

        void initType();

        bool IsBlackTurn = false;

        int m_BlackStoneUserIndex = -1;

        int m_WhiteStoneUserIndex = -1;

        int m_TurnIndex = -1;

    private:

        std::vector<std::vector<OmokPanPoint>> OmokPanPoints;

        ERROR_CODE CheckPos(int xPos, int yPos);
 
	};

}
