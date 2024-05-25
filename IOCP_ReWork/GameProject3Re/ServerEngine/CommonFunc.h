#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <Windows.h>
#include <tlhelp32.h>
#include <direct.h> 
#include <time.h>
#include <stdio.h> 
#include <io.h>


#define GET_BIT(X,Y) (((X) >> (Y-1)) & 1)
#define SET_BIT(X,Y) ((X) |= (1 << (Y-1)))
#define CLR_BIT(X,Y) ((X) &= (~(1<<Y-1)))


namespace CommonFunc
{

int32_t GetProcessorNum()
{
    int32_t nNum = 0;
#ifdef WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    nNum = sysInfo.dwNumberOfProcessors;
#else
    nNum = sysconf(_SC_NPROCESSORS_CONF);
#endif

    return nNum;
}

std::string GetCurrentExeDir()
{
    char szPath[1024] = { 0 };

#ifdef WIN32
    GetModuleFileNameA(NULL, szPath, 1024);
    char* p = strrchr(szPath, '\\');
#else
    readlink("/proc/self/exe", szPath, sizeof(szPath));
    char* p = strrchr(szPath, '/');
#endif
    * p = 0;
    return std::string(szPath);
}

std::string GetCurrentWorkDir()
{
    char szPath[1024];

#ifdef WIN32
    _getcwd(szPath, 1024);
#else
    getcwd(szPath, 1024);
#endif
    return std::string(szPath);
}

bool SetCurrentWorkDir(std::string strPath)
{
    if (strPath.empty())
    {
        strPath = GetCurrentExeDir();
    }

#ifdef WIN32
    SetCurrentDirectoryA(strPath.c_str());
#else
    chdir(strPath.c_str());
#endif
    return true;
}

bool CreateDir(std::string& strDir)
{
    int nRet = 0;
#ifdef WIN32
    nRet = _mkdir(strDir.c_str());
#else
    nRet = mkdir(strDir.c_str(), S_IRWXU);
#endif

    if (nRet == 0)
    {
        return true;
    }

    if (errno == EEXIST)
    {
        return true;
    }

    return false;
}

bool GetDirFiles(const char* pszDir, char* pszFileType, std::vector<std::string>& vtFileList, bool bRecursion)
{
    if (pszDir == nullptr || pszFileType == nullptr)
    {
        false;
    }

    char   szTem[1024] = { 0 };
    char   szDir[1024] = { 0 };
    strcpy_s(szTem, pszDir);
    if (szTem[strlen(szTem) - 1] != '\\' || szTem[strlen(szTem) - 1] != '/')
    {
        strcat_s(szTem, "/");
    }

    strcpy_s(szDir, szTem);
    strcpy_s(szDir, pszFileType);

#ifdef WIN32
    struct _finddata_t  tFileInfo = { 0 };
    long long hFile = _findfirst(szDir, &tFileInfo);
    if (hFile == -1)
    {
        return false;
    }

    do
    {
        if (strcmp(tFileInfo.name, ".") == 0 || strcmp(tFileInfo.name, "..") == 0)
        {
            continue;
        }

        if ((tFileInfo.attrib & _A_SUBDIR) && bRecursion)
        {
            char   szSub[1024] = { 0 };
            strcpy_s(szSub, pszDir);
            if (szSub[strlen(szSub) - 1] != '\\' || szSub[strlen(szSub) - 1] != '/')
            {
                strcat_s(szSub, "/");
            }
            strcat_s(szSub, tFileInfo.name);
            GetDirFiles(szSub, pszFileType, vtFileList, bRecursion);
        }
        else
        {
            vtFileList.push_back(std::string(szTem) + std::string(tFileInfo.name));
        }
    } while (_findnext(hFile, &tFileInfo) == 0);
    _findclose(hFile);

#else

    DIR* pDirInfo;
    struct dirent* tFileInfo;
    struct stat statbuf;
    if ((pDirInfo = opendir(pszDir)) == NULL)
    {
        return false;
    }

    while ((tFileInfo = readdir(pDirInfo)) != NULL)
    {
        if (strcmp(".", tFileInfo->d_name) == 0 || strcmp("..", tFileInfo->d_name) == 0)
        {
            continue;
        }

        char szTempDir[1024] = { 0 };
        strcpy(szTempDir, pszDir);
        strcat(szTempDir, tFileInfo->d_name);
        lstat(szTempDir, &statbuf);
        if ((S_IFDIR & statbuf.st_mode) && bRecursion)
        {
            char   szSub[1024] = { 0 };
            strcpy(szSub, pszDir);
            if (szSub[strlen(szSub) - 1] != '\\' || szSub[strlen(szSub) - 1] != '/')
            {
                strcat(szSub, "/");
            }
            strcat(szSub, tFileInfo->d_name);
            GetDirFiles(szSub, pszFileType, vtFileList, bRecursion);
        }
        else
        {
            vtFileList.push_back(std::string(szTem) + std::string(tFileInfo->d_name));
        }
    }

    closedir(pDirInfo);
#endif
    return true;
}

bool GetSubDirNames(const char* pszDir, const char* pszBegin, std::vector<std::string>& vtDirList, bool bRecursion)
{
    if (pszDir == NULL)
    {
        return false;
    }

    char   szTem[1024] = { 0 };
    char   szDir[1024] = { 0 };
    strcpy_s(szTem, pszDir);
    if (szTem[strlen(szTem) - 1] != '\\' || szTem[strlen(szTem) - 1] != '/')
    {
        strcat_s(szTem, "/*.*");
    }
    else
    {
        strcat_s(szTem, "*.*");
    }

    strcpy_s(szDir, szTem);

#ifdef WIN32
    struct _finddata_t  tFileInfo = { 0 };
    long long hFile = _findfirst(szDir, &tFileInfo);
    if (hFile == -1)
    {
        return false;
    }

    do
    {
        if (strcmp(tFileInfo.name, ".") == 0 || strcmp(tFileInfo.name, "..") == 0)
        {
            continue;
        }

        if (tFileInfo.attrib & _A_SUBDIR)
        {
            std::string strDirName = tFileInfo.name;

            if (strDirName.substr(0, strlen(pszBegin)) == std::string(pszBegin))
            {
                vtDirList.push_back(strDirName);
            }

            if (bRecursion)
            {
                char   szSub[1024] = { 0 };
                strcpy_s(szSub, pszDir);
                if (szSub[strlen(szSub) - 1] != '\\' || szSub[strlen(szSub) - 1] != '/')
                {
                    strcat_s(szSub, "/");
                }
                strcat_s(szSub, tFileInfo.name);
                GetSubDirNames(szSub, pszBegin, vtDirList, bRecursion);
            }
        }
    } while (_findnext(hFile, &tFileInfo) == 0);
    _findclose(hFile);

#else

    DIR* pDirInfo;
    struct dirent* tFileInfo;
    struct stat statbuf;
    if ((pDirInfo = opendir(pszDir)) == NULL)
    {
        return false;
    }

    while ((tFileInfo = readdir(pDirInfo)) != NULL)
    {
        if (strcmp(".", tFileInfo->d_name) == 0 || strcmp("..", tFileInfo->d_name) == 0)
        {
            continue;
        }

        char szTempDir[1024] = { 0 };
        strcpy(szTempDir, pszDir);
        strcat(szTempDir, tFileInfo->d_name);
        lstat(szTempDir, &statbuf);
        if (S_ISDIR(statbuf.st_mode))
        {
            std::string strDirName = tFileInfo->d_name;
            if (strDirName.substr(0, strlen(pszBegin)) == std::string(pszBegin))
            {
                vtDirList.push_back(strDirName);
            }

            if (bRecursion)
            {
                char   szSub[1024] = { 0 };
                strcpy(szSub, pszDir);
                if (szSub[strlen(szSub) - 1] != '\\' || szSub[strlen(szSub) - 1] != '/')
                {
                    strcat(szSub, "/");
                }
                strcat(szSub, tFileInfo->d_name);
                GetSubDirNames(szSub, pszBegin, vtDirList, bRecursion);
            }
        }
    }

    closedir(pDirInfo);
#endif
    return true;
}

uint64_t GetCurrTime() //현재 시간(초) 가져오기
{
    time_t t = time(0);
    return (uint64_t)t;
}

bool IsSameDay(uint64_t uTime)
{
#ifdef WIN32
    long time_zone = 0;
    _get_timezone(&time_zone);
    return ((uTime - time_zone) / 86400) == ((GetCurrTime() - time_zone) / 86400);
#else
    return ((uTime - timezone) / 86400) == ((GetCurrTime() - timezone) / 86400);
#endif
}

bool IsSameWeek(uint64_t uTime)
{
    time_t t = GetCurrTime();
    tm t_tmSrc;
    localtime_s(&t_tmSrc , &t);
    uint64_t SrcWeekBegin = (uint64_t)t - (t_tmSrc.tm_wday == 0 ? 6 : t_tmSrc.tm_wday - 1) * 86400 - t_tmSrc.tm_hour * 3600 - t_tmSrc.tm_min * 60 - t_tmSrc.tm_sec;

    t = uTime;
    tm t_tmDest;
    localtime_s(&t_tmDest, &t);
    uint64_t SrcWeekDest = (uint64_t)t - (t_tmDest.tm_wday == 0 ? 6 : t_tmDest.tm_wday - 1) * 86400 - t_tmDest.tm_hour * 3600 - t_tmDest.tm_min * 60 - t_tmDest.tm_sec;

    return (SrcWeekBegin - SrcWeekDest) / (86400 * 7) <= 0;
}

bool IsSameMonth(uint64_t uTime)
{
    time_t t = uTime;
    tm t_tmSrc;
    localtime_s(&t_tmSrc , &t);

    time_t t_mon = GetCurrTime();
    tm t_tmDest;
    localtime_s(&t_tmDest, &t_mon);

    return (t_tmSrc.tm_mon == t_tmDest.tm_mon);
}

int32_t DiffWeeks(uint64_t uTimeSrc, uint64_t uTimeDest)
{
    time_t t = uTimeSrc;
    tm t_tmSrc; 
    localtime_s(&t_tmSrc, &t);
    uint64_t SrcWeekBegin = (uint64_t)t - (t_tmSrc.tm_wday == 0 ? 6 : t_tmSrc.tm_wday - 1) * 86400 - t_tmSrc.tm_hour * 3600 - t_tmSrc.tm_min * 60 - t_tmSrc.tm_sec;

    t = uTimeDest;
    tm t_tmDest;
    localtime_s(&t_tmDest, &t);
    uint64_t SrcWeekDest = (uint64_t)t - (t_tmDest.tm_wday == 0 ? 6 : t_tmDest.tm_wday - 1) * 86400 - t_tmDest.tm_hour * 3600 - t_tmDest.tm_min * 60 - t_tmDest.tm_sec;

    return uTimeSrc > uTimeDest ? (int32_t)((SrcWeekBegin - SrcWeekDest) / (86400 * 7)) : (int32_t)((SrcWeekDest - SrcWeekBegin) / (86400 * 7));
}

int32_t DiffDays(uint64_t uTimeSrc, uint64_t uTimeDest)
{
#ifdef WIN32
    long time_zone = 0;
    _get_timezone(&time_zone);

    if (uTimeSrc > uTimeDest)
    {
        return (int32_t)((uTimeSrc - time_zone) / 86400 - (uTimeDest - time_zone) / 86400);
    }

    return (int32_t)((uTimeDest - time_zone) / 86400 - (uTimeSrc - time_zone) / 86400);
#else
    if (uTimeSrc > uTimeDest)
    {
        return (INT32)((uTimeSrc - timezone) / 86400 - (uTimeDest - timezone) / 86400);
    }

    return (INT32)((uTimeDest - timezone) / 86400 - (uTimeSrc - timezone) / 86400);
#endif
}

uint64_t GetCurMsTime() //현재 시간(밀리초) 가져오기
{
    auto time_now = std::chrono::system_clock::now();
    auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_now.time_since_epoch());
    return duration_in_ms.count();
}

tm GetCurrTmTime()
{
    time_t t = (time_t)GetCurrTime();

    tm _tm_time;
    localtime_s(&_tm_time, &t);

    return _tm_time;
}

uint64_t GetDayBeginTime(uint64_t uTime = 0) //하루의 시작 시점을 초 단위로 가져온다
{
    if (uTime == 0)
    {
        uTime = GetCurrTime();
    }

    time_t t = (time_t)uTime;
    tm t_tm;
    localtime_s(&t_tm, &t);
    t_tm.tm_hour = 0;
    t_tm.tm_min = 0;
    t_tm.tm_sec = 0;
    t = mktime(&t_tm);
    return (uint64_t)t;
}

uint64_t GetWeekBeginTime(uint64_t uTime = 0) //한 주의 시작 시간(초) 가져오기
{
    if (uTime == 0)
    {
        uTime = GetCurrTime();
    }

    time_t t = (time_t)uTime;
    tm t_tm;
    localtime_s(&t_tm, &t);
    return (uint64_t)t - (t_tm.tm_wday == 0 ? 6 : t_tm.tm_wday - 1) * 86400 - t_tm.tm_hour * 3600 - t_tm.tm_min * 60 - t_tm.tm_sec;
}

uint64_t GetMonthBeginTime(uint64_t uTime = 0)
{
    if (uTime == 0)
    {
        uTime = GetCurrTime();
    }

    time_t t = (time_t)uTime;
    tm t_tm;
    localtime_s(&t_tm, &t);
    tm newtm;
    newtm.tm_year = t_tm.tm_year;
    newtm.tm_mon = t_tm.tm_mon;
    newtm.tm_mday = 1;
    newtm.tm_hour = 0;
    newtm.tm_min = 0;
    newtm.tm_sec = 0;

    return mktime(&newtm);
}

uint64_t GetMonthRemainTime(uint64_t uTime = 0)
{
    if (uTime == 0)
    {
        uTime = GetCurrTime();
    }

    time_t t = (time_t)uTime;
    tm t_tm;
    localtime_s(&t_tm, &t);

    tm newtm;
    newtm.tm_mday = 1;
    newtm.tm_hour = 0;
    newtm.tm_min = 0;
    newtm.tm_sec = 0;

    if (t_tm.tm_mon == 11)
    {
        newtm.tm_year = t_tm.tm_year + 1;
        newtm.tm_mon = 0;
    }
    else
    {
        newtm.tm_year = t_tm.tm_year;
        newtm.tm_mon = t_tm.tm_mon + 1;
    }

    return mktime(&newtm) - t;
}

time_t YearTimeToSec(int32_t nYear, int32_t nMonth, int32_t nDay, int32_t nHour, int32_t nMin, int32_t nSec)
{
    time_t t = (time_t)GetCurrTime();
    tm t_tm;
    localtime_s(&t_tm, &t);

    tm newtm;
    newtm.tm_year = (nYear < 0) ? t_tm.tm_year : nYear - 1900;
    newtm.tm_mon = (nMonth < 0) ? t_tm.tm_mon : nMonth - 1;
    newtm.tm_mday = (nDay < 0) ? t_tm.tm_mday : nDay;
    newtm.tm_hour = (nHour < 0) ? t_tm.tm_hour : nHour;
    newtm.tm_min = (nMin < 0) ? t_tm.tm_min : nMin;
    newtm.tm_sec = (nSec < 0) ? t_tm.tm_sec : nSec;
    return mktime(&newtm);;
}

std::string TimeToString(time_t tTime)
{
    tm t_tm;
    localtime_s(&t_tm, &tTime);
    
    char szTime[128] = { 0 };
    strftime(szTime, 128, "%Y-%m-%d %H:%M:%S", &t_tm);
    return std::string(szTime);
}

time_t DateStringToTime(std::string strDate)
{
    if (strDate.size() < 14)
    {
        return 0;
    }

    int32_t nYear;
    int32_t nMonth;
    int32_t nDay;
    int32_t nHour;
    int32_t nMinute;
    int32_t nSecond;

    int32_t nRet = sscanf_s(strDate.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &nYear, &nMonth, &nDay, &nHour, &nMinute, &nSecond);
    if (nRet < 6)
    {
        return 0;
    }

    return YearTimeToSec(nYear, nMonth, nDay, nHour, nMinute, nSecond);
}

uint64_t GetTickCount()
{
#ifdef WIN32
    return ::GetTickCount64();
#else
    UINT64 uTickCount = 0;
    struct timespec on;
    if (0 == clock_gettime(CLOCK_MONOTONIC, &on))
    {
        uTickCount = on.tv_sec * 1000 + on.tv_nsec / 1000000;
    }

    return uTickCount;
#endif
}

int32_t GetCurThreadID()
{
    int32_t nThreadID = 0;
#ifdef WIN32
    nThreadID = ::GetCurrentThreadId();
#else
    nThreadID = (INT32)pthread_self();
#endif
    return nThreadID;
}

int32_t GetCurProcessID()
{
    int32_t nProcessID = 0;
#ifdef WIN32
    nProcessID = ::GetCurrentProcessId();
#else
    nProcessID = (INT32)getpid();
#endif
    return nProcessID;
}

void Sleep(int32_t nMilliseconds)
{
#ifdef WIN32
    ::Sleep(nMilliseconds);
#else
    struct timespec req;
    req.tv_sec = nMilliseconds / 1000;
    req.tv_nsec = nMilliseconds % 1000 * 1000000;
    nanosleep(&req, NULL);
#endif
}

int32_t GetFreePhysMemory()
{
    int32_t nFreeSize = 0;
#ifdef WIN32
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);

    GlobalMemoryStatusEx(&statex);

    nFreeSize = (int32_t)(statex.ullAvailPhys / 1024 / 1024);
#else
    INT32 nPageSize;
    INT32 nFreePages;
    nPageSize = sysconf(_SC_PAGESIZE) / 1024;
    nFreePages = sysconf(_SC_AVPHYS_PAGES) / 1024;
    nFreeSize = nFreePages * nPageSize;
#endif

    return nFreeSize;
}

int32_t GetRandNum(int32_t nType)
{
    if (nType >= 100 || nType < 0)
    {
        return 0;
    }

    static int32_t nRandIndex[100] = { 0 };
    static int32_t vtGlobalRankValue[10000];
    static bool  bInit = false;

    if (bInit == false)
    {
        bInit = true;
        int32_t nTempIndex;
        int32_t nTemp;
        for (int j = 0; j < 10000; j++)
        {
            vtGlobalRankValue[j] = j + 1;
        }

        for (int i = 0; i < 10000; i++)
        {
            nTempIndex = rand() % (i + 1);
            if (nTempIndex != i)
            {
                nTemp = vtGlobalRankValue[i];
                vtGlobalRankValue[i] = vtGlobalRankValue[nTempIndex];
                vtGlobalRankValue[nTempIndex] = nTemp;
            }
        }
    }

    return  vtGlobalRankValue[(nRandIndex[nType]++) % 10000];
}

int32_t GetLastError()
{
#ifdef WIN32
    return ::GetLastError();
#else
    return errno;
#endif
}

std::string GetLastErrorStr(int32_t nError)
{
    std::string strErrorText;
#ifdef WIN32
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, nError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);

    strErrorText = reinterpret_cast<char*>(lpMsgBuf);

    LocalFree(lpMsgBuf);
#else
    strErrorText = strerror(nError);
#endif

    return strErrorText;
}


std::string     GetLastErrorStr(int32_t nError);

bool KillProcess(int32_t nPid)
{
#ifdef WIN32
    HANDLE hPrc;
    if (0 == nPid)
    {
        return false;
    }

    hPrc = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)nPid);
    if (hPrc == NULL)
    {
        return true;
    }

    if (!TerminateProcess(hPrc, 0))
    {
        CloseHandle(hPrc);
        return false;
    }
    else
    {
        WaitForSingleObject(hPrc, 200);
    }
    CloseHandle(hPrc);
#else
    kill(nPid, SIGKILL);
#endif
    return true;
}

bool IsProcessExist(int32_t nPid)
{
#ifdef WIN32
    HANDLE hPrc;
    if (0 == nPid)
    {
        return false;
    }

    DWORD nExitCode = 0;
    hPrc = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)nPid);
    if (hPrc == NULL)
    {
        return false;
    }

    GetExitCodeProcess(hPrc, &nExitCode);
    if (nExitCode != STILL_ACTIVE)
    {
        return false;
    }
    CloseHandle(hPrc);
#else
    if (kill(nPid, 0) < 0)
    {
        return false;
    }

    if (errno == ESRCH)
    {
        return false;
    }
#endif
    return true;
}

bool IsAlreadyRun(std::string strSignName)
{
#ifdef WIN32
    HANDLE hMutex = NULL;
    hMutex = CreateMutexA(NULL, false, strSignName.c_str());
    if (hMutex != NULL)
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(hMutex);
            return true;
        }
    }
    return false;
#else
    INT32 fd;
    CHAR szbuf[32] = { 0 };

    std::string strLockFile = "/var/run/" + strSignName + ".pid";
    fd = open(strLockFile.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        return true;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) < 0)
    {
        close(fd);
        return true;
    }

    ftruncate(fd, 0);

    snprintf(szbuf, 32, "%ld", (long)getpid());

    write(fd, szbuf, strlen(szbuf) + 1);

    return false;
#endif
}

bool PrintColorText(CHAR* pSzText, int32_t nColor)
{
#ifdef WIN32
    switch (nColor)
    {
    case 1:
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        printf(pSzText);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    break;
    case 2:
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        printf(pSzText);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    break;
    case 3:
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        printf(pSzText);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    break;
    default:
    {
        printf(pSzText);
    }
    break;
    }
#else
    switch (nColor)
    {
    case 1:
    {
        printf("\033[1;31;40m%s\033[0m", pSzText);
    }
    break;
    case 2:
    {
        printf("\033[1;33;40m%s\033[0m", pSzText);
    }
    break;
    case 3:
    {
        printf("\033[1;32;40m%s\033[0m", pSzText);
    }
    break;
    default:
    {
        printf(pSzText);
    }
    break;
    }
#endif

    return true;
}

bool GetBitValue(uint64_t nValue, int32_t nPos)
{
    return ((nValue >> (nPos - 1)) & 1) > 0;
}

bool SetBitValue(uint64_t& nValue, int32_t nPos, bool bValue)
{
    if (bValue)
    {
        nValue |= (uint64_t)1 << (nPos - 1);
    }
    else
    {
        nValue &= ~((uint64_t)1 << (nPos - 1));
    }

    return true;
}


}


