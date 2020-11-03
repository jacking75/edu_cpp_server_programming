#include "Omok.h"


namespace ChatServerLib
{
    Omok::Omok() {}

    Omok::~Omok(){}
   
    const int OmokPanPointNumber = 19;
    bool IsBlackTurn;
    int OmokTurnUserIndex = -1;

	void Omok::init(int userIndex) 
	{
        for (int i = 0; i < OmokPanPointNumber; ++i)
        {
            for (int j = 0; j < OmokPanPointNumber; ++j)
            {
                OmokPanPoints[i, j]->Type = OmokPanPoint::PointType::None;
            }
        }
        IsBlackTurn = true;
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

           
    }

    bool Omok::IsUserTurn(int userIndex)
    {
        if (OmokTurnUserIndex == userIndex) return true;
        else return false;
    }

}

