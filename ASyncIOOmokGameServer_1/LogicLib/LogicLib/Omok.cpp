#pragma once

#include "Omok.h"
#include <iostream>
#include <algorithm>

namespace OmokServerLib
{
    const int timeOut = 20000;

    void Omok::init()
    {
        for (int i = 0; i < OmokLogic::OmokPanPointNumber; ++i)
        { 
            std::vector<PointType> elem;
            elem.resize(OmokLogic::OmokPanPointNumber);
            OmokPanPoints.push_back(elem);
        }

        IsBlackTurn = true;
        initType();
	}

    // TODO : fill 오류해결
    void Omok::initType()
    {
      //std::fill(&OmokPanPoints[0][0], &OmokPanPoints[OmokPanPointNumber - 1][OmokPanPointNumber], PointType::None);
       
      //이전코드
        for (int i = 0; i < OmokLogic::OmokPanPointNumber; ++i)
        {
            for (int j = 0; j < OmokLogic::OmokPanPointNumber; ++j)
            {
                OmokPanPoints[i][j] = PointType::None;
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

        if (point != PointType::None)
        {
            return ERROR_CODE::GAME_PUT_ALREADY_EXIST;
        }
    
        point = IsBlackTurn == true ? PointType::Black : PointType::White;

        IsBlackTurn = !IsBlackTurn;
        
        return ERROR_CODE::NONE;
    }

    ERROR_CODE Omok::CheckGameEnd(int xPos, int yPos)
    {
        auto pointType = IsBlackTurn == true ? PointType::White : PointType::Black;

        if (OmokLogic::ConfirmOmok(OmokPanPoints, xPos, yPos, pointType) == false)
        {
            return ERROR_CODE::NONE;
        }
        
        if (pointType == PointType::Black)
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
        if (xPos < 0 || yPos < 0 || xPos >= OmokLogic::OmokPanPointNumber || yPos >= OmokLogic::OmokPanPointNumber)
        {
            return ERROR_CODE::GAME_PUT_POS_ERROR;
        }

        return ERROR_CODE::NONE;
    }

    void Omok::SetUserTurnTime()
    {
        m_UserTurnTime = std::chrono::system_clock::now();
    }

    bool Omok::CheckTimeOut()
    {
        auto curTime = std::chrono::system_clock::now();
        auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_UserTurnTime);


        if (diffTime.count() >= timeOut)
        {
            return true;
        }

        return false;
    }


}

