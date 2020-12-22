#pragma once
#include "Omok.h"

namespace OmokServerLib
{
    //TODO 최흥배
    // 왜 static을 사용했나요?
    // -> 전에 static으로 해서 class 만들지 않아도 된다고 피드백 주셨었습니다 ~! 
    // -> 제가 생각한 것은 class를 만든 후 멤버 함수를 static으로 가지는 것을 생각했습니다.  아래 방식은 C언어 스타일 같아서요

    enum class PointType
    {
        None,
        Black = 1,
        White = 2,
    };

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