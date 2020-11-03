#pragma once

#include "Omok.h"
#include "OmokLogic.h"
#include "OmokPanPoint.h"

namespace ChatServerLib
{

    const int OmokPanPointNumber = 19;

    Omok::Omok() {}
    Omok::~Omok()
    {
        for (int count = 0; count < OmokPanPointNumber; ++count)
        {
            delete[] OmokPanPoints[count];
        }
        delete[] OmokPanPoints;
    }
   
  
    void Omok::init(int userIndex)
    {
        OmokPanPoints = new OmokPanPoint * [OmokPanPointNumber];

        for (int count = 0; count < OmokPanPointNumber; ++count)
        {
            OmokPanPoints[count] = new OmokPanPoint[OmokPanPointNumber];
        }

        for (int i = 0; i < OmokPanPointNumber; ++i)
        {
            for (int j = 0; j < OmokPanPointNumber; ++j)
            {
                OmokPanPoints[i][j].Type = OmokPanPoint::PointType::None;
            }
        }
        IsBlackTurn = !IsBlackTurn;
        OmokTurnUserIndex = userIndex;

	}

    NServerNetLib::ERROR_CODE Omok::GamePutStone(int userIndex, int xPos, int yPos)
    {
        if (IsUserTurn(userIndex) == false)
        {
            return NServerNetLib::ERROR_CODE::GAME_NOT_YOUR_TURN;
        }

        auto point = OmokPanPoints[yPos][xPos];
        if (point.Type != OmokPanPoint::PointType::None)
        {
            return NServerNetLib::ERROR_CODE::GAME_PUT_ALREADY_EXIST;
        }

        OmokPanPoint::PointType pointType = OmokPanPoint::PointType::None;
        pointType = IsBlackTurn == true ? OmokPanPoint::PointType::Black : OmokPanPoint::PointType::White;
        OmokLogic logic;

        if (logic.ConfirmOmok(OmokPanPoints,xPos, yPos, pointType) == true)
        {
            return  NServerNetLib::ERROR_CODE::GAME_PUT_SAM_SAM;
        }
        
        point.Type = pointType;
        IsBlackTurn = !IsBlackTurn;
        
        return NServerNetLib::ERROR_CODE::NONE;
    }


    bool Omok::IsUserTurn(int userIndex)
    {
        if (OmokTurnUserIndex == userIndex) return true;
        else return false;
    }

    NServerNetLib::ERROR_CODE Omok::CheckGameEnd(int xPos, int yPos)
    {
        auto pointType = IsBlackTurn == true ? OmokPanPoint::PointType::White : OmokPanPoint::PointType::Black;
        OmokLogic logic;
        if (logic.ConfirmOmok(OmokPanPoints, xPos, yPos, pointType) == true)
        {
            if (pointType == OmokPanPoint::PointType::Black)
            {
                return NServerNetLib::ERROR_CODE::GAME_RESULT_BLACK_WIN;
            }
            else 
            {
                return NServerNetLib::ERROR_CODE::GAME_RESULT_WHITE_WIN;
            }
            return NServerNetLib::ERROR_CODE::NONE;
        }

    }



}

