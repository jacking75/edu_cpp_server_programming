#pragma once

#include "Omok.h"
#include "OmokLogic.h"
#include "OmokPanPoint.h"

namespace ChatServerLib
{

    bool OmokLogic::ConfirmOmok(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType)
    {
        if (ConfirmVertical(omokPanPoints, x, y, pointType) == 5
            || ConfirmHorizontal(omokPanPoints, x, y, pointType) == 5
            || ConfirmDiagonal(omokPanPoints, x, y, pointType) == 5
            || ConfirmOpDiagonal(omokPanPoints, x, y, pointType) == 5)
        {
            return true;
        }

        return false;
    }

    bool OmokLogic::ConfirmThreeThree(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType)
    {
        int cnt = 0;

        if (ConfirmHorizontal(omokPanPoints, x, y, pointType) == 3)
        {
            ++cnt;
        }
        if (ConfirmVertical(omokPanPoints, x, y, pointType) == 3)
        {
            ++cnt;
        }
        if (ConfirmDiagonal(omokPanPoints, x, y, pointType) == 3)
        {
            ++cnt;
        }
        if (ConfirmOpDiagonal(omokPanPoints, x, y, pointType) == 3)
        {
            ++cnt;
        }

        return cnt > 1 ? true : false;
    }

    int OmokLogic::ConfirmHorizontal(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType)
    {
        int cnt = 1;

        for (int i = 1; i <= 5; ++i)
        {
            if (x + i <= 18
                && pointType == omokPanPoints[y][x + i].Type)
            {
                ++cnt;
            }
            else
            {
                break;
            }
        }

        for (int i = 1; i <= 5; ++i)
        {
            if (x - i >= 0
                && pointType == omokPanPoints[y][x - i].Type)
            {
                ++cnt;
            }
            else
            {
                break;
            }
        }

        return cnt;
    }
   int OmokLogic::ConfirmVertical(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType)
    {
        int cnt = 1;

        for (int i = 1; i <= 5; ++i)
        {
            if (y + i <= 18
                && pointType == omokPanPoints[y + i][x].Type)
            {
                ++cnt;
            }
            else
            {
                break;
            }
        }

        for (int i = 1; i <= 5; ++i)
        {
            if (y - i >= 0
                && pointType == omokPanPoints[y - i][x].Type)
            {
                ++cnt;
            }
            else
            {
                break;
            }
        }

        return cnt;
    }
   int OmokLogic::ConfirmDiagonal(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType)
   {
       int cnt = 1;

       for (int i = 1; i <= 5; ++i)
       {
           if (x + i <= 18 && y + i <= 18
               && pointType == omokPanPoints[y + i][x + i].Type)
           {
               ++cnt;
           }
           else
           {
               break;
           }
       }

       for (int i = 1; i <= 5; ++i)
       {
           if (x - i >= 0 && y - i >= 0
               && pointType == omokPanPoints[y - i][x - i].Type)
           {
               ++cnt;
           }
           else
           {
               break;
           }
       }

       return cnt;
   }

   int OmokLogic::ConfirmOpDiagonal(OmokPanPoint** omokPanPoints, int x, int y, OmokPanPoint::PointType pointType)
   {
       int cnt = 1;

       for (int i = 1; i <= 5; ++i)
       {
           if (x - i >= 0 && y + i <= 18
               && pointType == omokPanPoints[y + i][x - i].Type)
           {
               ++cnt;
           }
           else
           {
               break;
           }
       }

       for (int i = 1; i <= 5; ++i)
       {
           if (x + i <= 18 && y - i >= 0
               && pointType == omokPanPoints[y - i][x + i].Type)
           {
               ++cnt;
           }
           else
           {
               break;
           }
       }

       return cnt;
   }
   
}