#pragma once

#include "Omok.h"
#include "OmokLogic.h"
#include <iostream>
#include <chrono>

namespace OmokServerLib
{
    void Omok::init()
    {
        for (int i = 0; i < OmokPanPointNumber; ++i)
        { 
            std::vector<OmokPanPoint> elem; 
            elem.resize(OmokPanPointNumber); 
            OmokPanPoints.push_back(elem);
        }

        IsBlackTurn = true;
        initType();
	}

    void Omok::initType()
    {
        for (int i = 0; i < OmokPanPointNumber; ++i)
        {
            for (int j = 0; j < OmokPanPointNumber; ++j)
            {
                OmokPanPoints[i][j].Type = OmokPanPoint::PointType::None;
            }
        }
    }

    ERROR_CODE Omok::GamePutStone(int xPos, int yPos)
    {   
        auto posResult = CheckPos(xPos, yPos);

        if (posResult != ERROR_CODE::NONE)
        {
            return posResult;
        }

        auto &point = OmokPanPoints[yPos][xPos];

        if (point.Type != OmokPanPoint::PointType::None)
        {
            return ERROR_CODE::GAME_PUT_ALREADY_EXIST;
        }
    
        point.Type = IsBlackTurn == true ? OmokPanPoint::PointType::Black : OmokPanPoint::PointType::White;

        IsBlackTurn = !IsBlackTurn;
        
        return ERROR_CODE::NONE;
    }

    ERROR_CODE Omok::CheckGameEnd(int xPos, int yPos)
    {
        auto pointType = IsBlackTurn == true ? OmokPanPoint::PointType::White : OmokPanPoint::PointType::Black;

        if (ConfirmOmok(OmokPanPoints, xPos, yPos, pointType) == false)
        {
            return ERROR_CODE::NONE;
        }
        
        if (pointType == OmokPanPoint::PointType::Black)
        {
            return ERROR_CODE::GAME_RESULT_BLACK_WIN;
        }
        else 
        {
            return ERROR_CODE::GAME_RESULT_WHITE_WIN;
        }
         
    }

    ERROR_CODE Omok::CheckPos(int xPos, int yPos)
    {
        if (xPos < 0 || yPos < 0 || xPos >= OmokPanPointNumber || yPos >= OmokPanPointNumber)
        {
            return ERROR_CODE::GAME_PUT_POS_ERROR;
        }

        return ERROR_CODE::NONE;
    }

    void Omok::SetUserTurnTime()
    {
        auto curTime = std::chrono::system_clock::now();
        m_UserTurnTime = std::chrono::system_clock::to_time_t(curTime);
    }

    void Omok::ClearUserTurnTime()
    {
        m_UserTurnTime = -1;
    }

    bool Omok::CheckTimeOut()
    {
        auto curTime = std::chrono::system_clock::now();
        auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

        auto diff = curSecTime - m_UserTurnTime;
        if (diff >= 5)
        {
            return true;
        }

        return false;
    }


}

