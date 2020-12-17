#pragma once

#include "ErrorCode.h"
#include "OmokPanPoint.h"
#include <vector>
#include <chrono>

namespace OmokServerLib
{
    class Omok {


    public:
        Omok() = default;
        ~Omok() = default;

        void init();

        ERROR_CODE CheckGameEnd(int xPos, int yPos);

        ERROR_CODE GamePutStone(int xPos, int yPos);

        bool CheckTimeOut();

        void initType();

        bool IsBlackTurn = false;

        int m_BlackStoneUserIndex = -1;

        int m_WhiteStoneUserIndex = -1;

        int m_TurnIndex = -1;

        void SetUserTurnTime();

    private:
        const int timeOut = 20000;

        std::chrono::system_clock::time_point m_UserTurnTime = std::chrono::system_clock::now();

        std::vector<std::vector<PointType>> OmokPanPoints;

        ERROR_CODE CheckPos(int xPos, int yPos);

    };

}
