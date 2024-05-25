#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>


namespace CommonConvert
{
int32_t StringToInt(const char* pStr) noexcept 
{
    if (pStr == nullptr)
    {
        return 0;
    }

    return atoi(pStr);
}

int64_t StringToInt64(const char* pStr) noexcept
{
    if (pStr == NULL)
    {
        return 0;
    }

#ifdef WIN32
    return _atoi64(pStr);
#else
    return atoll(pStr);
#endif
}

float StringToFloat(const char* pStr) noexcept
{
    if (pStr == NULL)
    {
        return 0;
    }

    return (float)atof(pStr);
}

double StringToDouble(const char* pStr) noexcept
{
    if (pStr == NULL)
    {
        return 0;
    }

    return strtod(pStr, NULL);
}

std::string DoubleToString(double dValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%f", dValue);

    return std::string(szValue);
}

std::string IntToString(uint32_t nValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%d", nValue);

    return std::string(szValue);
}

std::string IntToString(int32_t nValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%d", nValue);

    return std::string(szValue);
}

std::string IntToString(uint64_t nValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%lld", nValue);

    return std::string(szValue);
}

std::string IntToString(int64_t nValue)
{
    char szValue[64] = { 0 };

    snprintf(szValue, 64, "%lld", nValue);

    return std::string(szValue);
}

std::string FloatToString(float fValue, int32_t nPrecision = -1, bool bRound = false)
{
    char szValue[64] = { 0 };

    if ((bRound) && (nPrecision > 0) && (nPrecision < 6))
    {
        float fRoundValue = 5;

        for (int i = 0; i < nPrecision + 1; i++)
        {
            fRoundValue *= 0.1f;
        }

        fValue += fRoundValue;
    }

    snprintf(szValue, 64, "%f", fValue);

    char* pChar = strchr(szValue, '.');
    if (pChar == NULL)
    {
        return std::string(szValue);
    }

    *(pChar + nPrecision + 1) = '\0';

    return std::string(szValue);
}

bool  StringToPos(const char* pStr, float& x, float& y, float& z)
{
    if (pStr == NULL)
    {
        return false;
    }

    char szTempBuf[256] = { 0 };
    strncpy_s(szTempBuf, pStr, strlen(pStr));

    char* pPos = strchr(szTempBuf, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    x = CommonConvert::StringToFloat(szTempBuf + 1);

    char* pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    y = CommonConvert::StringToFloat(pOldPos);

    pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ')');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    z = CommonConvert::StringToFloat(pOldPos);

    return true;
}

bool  StringToBox(const char* pStr, float& left, float& top, float& right, float& bottom)
{
    if (pStr == NULL)
    {
        return false;
    }

    char szTempBuf[256] = { 0 };
    strncpy_s(szTempBuf, pStr, strlen(pStr));

    char* pPos = strchr(szTempBuf, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    left = CommonConvert::StringToFloat(szTempBuf + 1);

    char* pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    top = CommonConvert::StringToFloat(pOldPos);

    pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ',');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    right = CommonConvert::StringToFloat(pOldPos);

    pOldPos = pPos + 1;
    pPos = strchr(pPos + 1, ')');
    if (pPos == NULL)
    {
        return false;
    }
    *pPos = 0;
    bottom = CommonConvert::StringToFloat(pOldPos);

    return true;
}

bool IsTextUTF8(const char* str, int32_t nLength) noexcept 
{
    int32_t i;
    int32_t nBytes = 0;//UFT8은 1~6바이트, ASCII는 1바이트로 인코딩할 수 있다
    uint8_t chr;
    auto bAllAscii = true; //모두 ASCII인 경우 UTF-8이 아니다

    for (i = 0; i < nLength; i++)
    {
        chr = *(str + i);
        if ((chr & 0x80) != 0) // ASCII 인코딩인지 판단하고, 그렇지 않은 경우 7비트 인코딩을 사용하는 UTF-8, ASCII 이지만 바이트 저장소가 있는 경우 가장 높은 비트가 0으로 표시된 o0xxxxxxx일 수 있다
        {
            bAllAscii = false;
        }
        if (nBytes == 0) //ASCII가 아닌 경우 멀티바이트 문자이어야 하며, 바이트 수를 세어야 한다
        {
            if (chr >= 0x80)
            {
                if (chr >= 0xFC && chr <= 0xFD)
                {
                    nBytes = 6;
                }
                else if (chr >= 0xF8)
                {
                    nBytes = 5;
                }
                else if (chr >= 0xF0)
                {
                    nBytes = 4;
                }
                else if (chr >= 0xE0)
                {
                    nBytes = 3;
                }
                else if (chr >= 0xC0)
                {
                    nBytes = 2;
                }
                else
                {
                    return false;
                }
                nBytes--;
            }
        }
        else // 멀티바이트 문자의 첫 번째 바이트가 아닌 바이트는 10xxxxxx 여야한다
        {
            if ((chr & 0xC0) != 0x80)
            {
                return false;
            }
            nBytes--;
        }
    }
    if (nBytes > 0) // 미반품에 대한 규칙
    {
        return false;
    }
    if (bAllAscii) // 모두 ASCII인 경우 UTF-8이 아니다
    {
        return true;
    }
    return true;
}

int32_t GetValidUtf8Length(char* pStr, int32_t nLen)
{
    int32_t nCurPos = 0;
    auto bUtfStart = false;
    int32_t nUtfLen = 0;
    int32_t nBkLen = 0;
    for (int i = 0; i < nLen; i++)
    {
        uint8_t nCurChar = *(pStr + i);
        if (!bUtfStart)
        {
            if ((nCurChar & 0x80) == 0)
            {
                nUtfLen = 0;
                nCurPos += 1;
            }
            else if (nCurChar >= 0xFC && nCurChar <= 0xFD)
            {
                nBkLen = nUtfLen = 6;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xF8)
            {
                nBkLen = nUtfLen = 5;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xF0)
            {
                nBkLen = nUtfLen = 4;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xE0)
            {
                nBkLen = nUtfLen = 3;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else if (nCurChar >= 0xC0)
            {
                nBkLen = nUtfLen = 2;
                bUtfStart = true;
                nUtfLen -= 1;
            }
            else
            {
                return nCurPos;
            }
        }
        else
        {
            if ((nCurChar & 0xC0) != 0x80)
            {
                return nCurPos;
            }
            else
            {
                nUtfLen -= 1;
                if (nUtfLen <= 0)
                {
                    bUtfStart = false;
                    nCurPos += nBkLen;
                    nBkLen = nUtfLen = 0;
                }
            }
        }
    }

    return nCurPos;
}

std::string TruncateUtf8(char* pStr, int32_t nLen)
{
    int32_t nUtf8Len = GetValidUtf8Length(pStr, nLen);

    return std::string(pStr, nUtf8Len);
}

bool SpliteString(std::string strSrc,  std::string strDelim, std::vector<std::string>& vtStr)
{
    vtStr.clear();
    if (strDelim.empty())
    {
        vtStr.push_back(strSrc);
        return true;
    }

    if (strSrc.size() <= 0)
    {
        return true;
    }

    std::string::iterator subStart, subEnd;
    subStart = strSrc.begin();
    while (true)
    {
        subEnd = std::search(subStart, strSrc.end(), strDelim.begin(), strDelim.end());
        std::string temp(subStart, subEnd);
        if (!temp.empty())
        {
            vtStr.push_back(temp);
        }
        if (subEnd == strSrc.end())
        {
            break;
        }
        subStart = subEnd + strDelim.size();
    }

    return true;
}

bool SpliteStringByBlank(std::string strSrc, std::vector<std::string>& vtStr)
{
    vtStr.clear();
    auto bWordStart = false;
    std::string::size_type posStart;
    std::string::size_type posCur = 0;

    while (posCur < strSrc.size())
    {
        if (strSrc.at(posCur) == ' ' || strSrc.at(posCur) == '\t')
        {
            if (!bWordStart)
            {
                posCur += 1;
                continue;
            }

            if (posCur > posStart)
            {
                vtStr.push_back(strSrc.substr(posStart, posCur - posStart));
            }

            bWordStart = false;
            posStart = 0;
            posCur += 1;
        }
        else
        {
            if (!bWordStart)
            {
                posStart = posCur;
                bWordStart = true;
            }
            posCur += 1;
        }
    }

    return true;
}

bool SpliteString(std::string strSrc, char cDelim, std::vector<std::string>& vtStr)
{
    vtStr.clear();

    std::string::size_type posStart, posEnd;
    posEnd = strSrc.find(cDelim);
    posStart = 0;
    while (std::string::npos != posEnd)
    {
        vtStr.emplace_back(strSrc.substr(posStart, posEnd - posStart));

        posStart = posEnd + 1;
        posEnd = strSrc.find(cDelim, posStart);
    }

    if (posStart != strSrc.length())
    {
        vtStr.emplace_back(strSrc.substr(posStart));
    }

    return true;
}

bool ReplaceString(std::string& strSrc, const std::string& pattern, const std::string& newpat)
{
    const size_t nsize = newpat.size();
    const size_t psize = pattern.size();

    for (size_t pos = strSrc.find(pattern, 0); 
        pos != std::string::npos; 
        pos = strSrc.find(pattern, pos + nsize))
    {
        strSrc.replace(pos, psize, newpat);
    }

    return true;
}

bool StringToVector(const char* pStrValue, std::vector<int32_t>& vtInt, char cDelim = ',')
{
    if (pStrValue == NULL)
    {
        return false;
    }

    if (strlen(pStrValue) > 1024)
    {
        return false;
    }

    char szBuf[1024] = { 0 };
    strncpy_s(szBuf, pStrValue, 1024);

    char* pBeginPos = szBuf;
    char* pEndPos = strchr(pBeginPos, cDelim);

    if (pBeginPos == pEndPos)
    {
        pBeginPos += 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    while (pEndPos != NULL)
    {
        int32_t nValue = StringToInt(pBeginPos);
        vtInt.push_back(nValue);
        pBeginPos = pEndPos + 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    if (*pBeginPos != 0)
    {
        int32_t nValue = StringToInt(pBeginPos);
        vtInt.push_back(nValue);
    }

    return true;
}

bool StringToVector(const char* pStrValue, int32_t IntVector[], int32_t TotalSize, char cDelim = ',')
{
    if (pStrValue == NULL)
    {
        return false;
    }

    char szBuf[1024] = { 0 };
    strncpy_s(szBuf, pStrValue, 1024);

    char* pBeginPos = szBuf;
    char* pEndPos = strchr(pBeginPos, cDelim);

    if (pBeginPos == pEndPos)
    {
        pBeginPos += 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    int32_t nIndex = 0;
    while (pEndPos != NULL)
    {
        //*pEndPos = 0;
        IntVector[nIndex++] = StringToInt(pBeginPos);
        if (nIndex >= TotalSize)
        {
            return true;
        }

        pBeginPos = pEndPos + 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    if (*pBeginPos != 0 && nIndex < TotalSize)
    {
        IntVector[nIndex++] = StringToInt(pBeginPos);
    }

    return true;
}

bool StringToVector(const char* pStrValue, float FloatVector[], int32_t TotalSize, char cDelim = ',')
{
    if (pStrValue == NULL)
    {
        return false;
    }

    char szBuf[1024] = { 0 };
    strncpy_s(szBuf, pStrValue, 1024);

    char* pBeginPos = szBuf;
    char* pEndPos = strchr(pBeginPos, cDelim);

    if (pBeginPos == pEndPos)
    {
        pBeginPos += 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    int32_t nIndex = 0;
    while (pEndPos != NULL)
    {
        FloatVector[nIndex++] = StringToFloat(pBeginPos);
        if (nIndex >= TotalSize)
        {
            return true;
        }

        pBeginPos = pEndPos + 1;
        pEndPos = strchr(pBeginPos, cDelim);
    }

    if (*pBeginPos != 0 && nIndex < TotalSize)
    {
        FloatVector[nIndex++] = StringToFloat(pBeginPos);
    }

    return true;
}

int32_t VersionToInt(const std::string& strVersion)
{
    int32_t nValue[3] = { 0 };
    StringToVector(strVersion.c_str(), nValue, 3, '.');
    return nValue[0] * 1000000 + nValue[1] * 1000 + nValue[2];
}

int32_t CountSymbol(const char* pStr, char cSymbol)
{
    if (pStr == NULL)
    {
        return 0;
    }

    int32_t nCount = 0;

    const char* pTemp = pStr;
    while (*pTemp != '\0')
    {
        if (*pTemp == cSymbol)
        {
            nCount += 1;
        }

        pTemp += 1;
    }

    return nCount;
}

bool HasSymbol(const char* pStr, const char* pszSymbol)
{
    if (pStr == NULL || pszSymbol == NULL)
    {
        return false;
    }

    const char* pSym = pszSymbol;
    while (*pSym != '\0')
    {
        const char* pTemp = pStr;
        while (*pTemp != '\0')
        {
            if (*pTemp == *pSym)
            {
                return true;
            }

            pTemp += 1;
        }

        pSym += 1;
    }

    return false;
}

bool StringTrim(std::string& strValue)
{
    if (!strValue.empty())
    {
        strValue.erase(0, strValue.find_first_not_of((" \n\r\t")));
        strValue.erase(strValue.find_last_not_of((" \n\r\t")) + 1);
    }
    
    return true;
}

// nLen의 크기는 pszDest의 크기보다 작거나 같아야 한다
bool StrCopy(char* pszDest, const char* pszSrc, int32_t nLen)
{
    if (pszDest == NULL || pszSrc == NULL)
    {
        return false;
    }

    if (nLen <= 0)
    {
        return false;
    }

    int32_t nSrcLen = (int32_t)strlen(pszSrc);

    int32_t nCpyLen = 0;
    if ((nLen - 1) > nSrcLen)
    {
        nCpyLen = nSrcLen;
    }
    else
    {
        nCpyLen = nLen - 1;
    }

    // 이 방법은 종결문자를 자동으로 추가하지 않는다
    strncpy_s(pszDest, nLen, pszSrc, nCpyLen);
    pszDest[nCpyLen] = '\0';

    return true;
}

bool EscapeString(char* pszDest, int32_t nLen)
{
    for (int i = 0; i < nLen; i++)
    {
        if (pszDest[i] == '\'')
        {
            pszDest[i] = ' ';
        }

        if (pszDest[i] == '\\')
        {
            pszDest[i] = ' ';
        }

        if (pszDest[i] == '\"')
        {
            pszDest[i] = ' ';
        }

        if (pszDest[i] == '\0')
        {
            break;
        }
    }

    return true;
}

bool FromHex(const char& x)
{
    return isdigit(x) ? x - '0' : x - 'A' + 10;
}



}

