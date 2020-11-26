#pragma once

#include "Omok.h"
#include "OmokLogic.h"
#include <iostream>

namespace ChatServerLib
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
        
        IsBlackTurn = true;
    }

    void Omok::printTest() {

        for (int i = 0; i < OmokPanPointNumber; ++i)
        {
            for (int j = 0; j < OmokPanPointNumber; ++j)
            {
                std::cout << (int)OmokPanPoints[j][i].Type << " ";
            }
            std::cout << "\n"; 
        }
        std::cout << "\n"; std::cout << "\n";
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


}

