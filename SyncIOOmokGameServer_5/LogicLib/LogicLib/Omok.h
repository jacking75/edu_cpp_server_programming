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
        //TODO 최흥배
        // 아래 상수는 Omok.cpp에서만 사용한다면 cpp에 선언하세요. 헤더 파일에는 가급적 선언 안하는 것이 좋습니다.
        const int timeOut = 20000;

        std::chrono::system_clock::time_point m_UserTurnTime = std::chrono::system_clock::now();

        std::vector<std::vector<PointType>> OmokPanPoints;

        ERROR_CODE CheckPos(int xPos, int yPos);

    };

}
