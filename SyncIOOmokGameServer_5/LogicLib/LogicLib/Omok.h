#pragma once

#include "ErrorCode.h"
#include "OmokPanPoint.h"
#include <vector>

namespace OmokServerLib
{
    class Omok {


    public:
        Omok() = default;
        ~Omok() = default;

        void init();

        void printTest();

        ERROR_CODE CheckGameEnd(int xPos, int yPos);

        ERROR_CODE GamePutStone(int xPos, int yPos);

        bool CheckTimeOut();

        void initType();

        bool IsBlackTurn = false;

        int m_BlackStoneUserIndex = -1;

        int m_WhiteStoneUserIndex = -1;

        int m_TurnIndex = -1;

        void SetUserTurnTime();

        void ClearUserTurnTime();

    private:

        int64_t m_UserTurnTime;

        std::vector<std::vector<OmokPanPoint>> OmokPanPoints;

        ERROR_CODE CheckPos(int xPos, int yPos);

    };

}
