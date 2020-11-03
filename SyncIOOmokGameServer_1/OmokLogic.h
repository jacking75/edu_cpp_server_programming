#pragma once
#include "OmokPanPoint.h"
namespace ChatServerLib
{
    class OmokLogic
    {
    public:
        bool ConfirmOmok(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType);
        bool ConfirmThreeThree(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType);
        int ConfirmHorizontal(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType);
        int ConfirmVertical(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType);
        int ConfirmDiagonal(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType);
        int ConfirmOpDiagonal(OmokPanPoint** omokPanPointsint, int x, int y, OmokPanPoint::PointType pointType);
    };

}