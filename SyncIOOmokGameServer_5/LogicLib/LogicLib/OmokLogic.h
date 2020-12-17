#pragma once
#include "OmokPanPoint.h"
#include "Omok.h"

namespace OmokServerLib
{
    static const int OmokPanPointNumber = 19;

    static int ConfirmHorizontal(std::vector<std::vector<PointType>> &omokPanPoints, int x, int y, PointType pointType)
    {
        int cnt = 1;

        for (int i = 1; i <= 5; ++i)
        {
            if (x + i < OmokPanPointNumber && pointType == omokPanPoints[y][x + i])
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
            if (x - i >= 0 && pointType == omokPanPoints[y][x - i])
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
    static int ConfirmVertical(std::vector<std::vector<PointType>> &omokPanPoints, int x, int y, PointType pointType)
    {
        int cnt = 1;

        for (int i = 1; i <= 5; ++i)
        {
            if (y + i < OmokPanPointNumber && pointType == omokPanPoints[y + i][x])
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
            if (y - i >= 0 && pointType == omokPanPoints[y - i][x])
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
    static int ConfirmDiagonal(std::vector<std::vector<PointType>> &omokPanPoints, int x, int y, PointType pointType)
    {
        int cnt = 1;

        for (int i = 1; i <= 5; ++i)
        {
            if (x + i < OmokPanPointNumber && y + i < OmokPanPointNumber && pointType == omokPanPoints[y + i][x + i])
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
            if (x - i >= 0 && y - i >= 0 && pointType == omokPanPoints[y - i][x - i])
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

    static int ConfirmOpDiagonal(std::vector<std::vector<PointType>> &omokPanPoints, int x, int y, PointType pointType)
    {
        int cnt = 1;

        for (int i = 1; i <= 5; ++i)
        {
            if (x - i >= 0 && y + i < OmokPanPointNumber && pointType == omokPanPoints[y + i][x - i])
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
            if (x + i < OmokPanPointNumber && y - i >= 0 && pointType == omokPanPoints[y - i][x + i])
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

    static bool ConfirmOmok(std::vector<std::vector<PointType>> &omokPanPoints, int x, int y, PointType pointType)
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

    
}