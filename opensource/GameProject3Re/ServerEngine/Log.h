#pragma once

#include "CommonFunc.h"

#include <string>
#include <mutex> 


namespace hbServerEngine
{
    enum class LogLevel
    {
        HiInfo,
        Error,
        Warn,
        Info,
        None
    };


    class CLog
    {
    private:
        CLog(void)
        {
            m_LogLevel = LogLevel::None;
        }

        ~CLog(void)
        {
            Close();
        }

    public:
        static CLog* GetInstancePtr()
        {
            static CLog Log;
            return &Log;
        }

        bool Start(std::string strPrefix, std::string strLogDir = "log")
        {
            if (!CommonFunc::CreateDir(strLogDir))
            {
                return false;
            }
            m_strPrefix = strPrefix;
            m_strLogDir = strLogDir;

#ifdef WIN32
            SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS);
#endif
            m_pLogFile = NULL;

            tm CurTime = CommonFunc::GetCurrTmTime();

            char szFileName[512];

            snprintf(szFileName, 512, "%s/%s-%02d%02d%02d-%02d%02d%02d.log", strLogDir.c_str(), strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);
        
            if (fopen_s(&m_pLogFile, szFileName, "w+") != 0)
            {
                return false;
            }

            m_LogCount = 0;

            return true;
        }

        bool Close()
        {
            if (m_pLogFile == NULL)
            {
                return false;
            }

            fflush(m_pLogFile);

            fclose(m_pLogFile);

            m_pLogFile = NULL;

            return true;
        }

        void LogHiInfo(const char* lpszFormat, ...)
        {
            if (m_LogLevel < LogLevel::HiInfo)
            {
                return;
            }

            if (m_pLogFile == NULL)
            {
                return;
            }

            CHAR szLog[2048] = { 0 };

            tm CurTime = CommonFunc::GetCurrTmTime();
            snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][I] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

            va_list argList;
            va_start(argList, lpszFormat);
            vsnprintf(szLog + 28, 1024 - 28, lpszFormat, argList);
            va_end(argList);

            strncat_s(szLog, "\n", 3);

            m_WriteMutex.lock();
            fputs(szLog, m_pLogFile);
            fflush(m_pLogFile);
            m_LogCount++;
            CommonFunc::PrintColorText(szLog, (int32_t)LogLevel::Info);
            CheckAndCreate();
            m_WriteMutex.unlock();

            return;
        }

        void LogWarn(const char* lpszFormat, ...)
        {
            if (m_LogLevel < LogLevel::Warn)
            {
                return;
            }

            if (m_pLogFile == NULL)
            {
                return;
            }

            tm CurTime = CommonFunc::GetCurrTmTime();

            CHAR szLog[2048] = { 0 };
            snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][W] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

            va_list argList;
            va_start(argList, lpszFormat);
            vsnprintf(szLog + 28, 1024 - 28, lpszFormat, argList);
            va_end(argList);

            strncat_s(szLog, "\n", 3);

            m_WriteMutex.lock();
            fputs(szLog, m_pLogFile);
            fflush(m_pLogFile);
            m_LogCount++;
            CommonFunc::PrintColorText(szLog, (int32_t)LogLevel::Warn);
            CheckAndCreate();
            m_WriteMutex.unlock();

            return;
        }

        void LogError(const char* lpszFormat, ...)
        {
            if (m_LogLevel < LogLevel::Error)
            {
                return;
            }

            if (m_pLogFile == NULL)
            {
                return;
            }

            CHAR szLog[2048] = { 0 };
            tm CurTime = CommonFunc::GetCurrTmTime();
            snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][E] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

            va_list argList;
            va_start(argList, lpszFormat);
            vsnprintf(szLog + 28, 1024 - 28, lpszFormat, argList);
            va_end(argList);

            strncat_s(szLog, "\n", 3);

            m_WriteMutex.lock();
            fputs(szLog, m_pLogFile);
            fflush(m_pLogFile);
            m_LogCount++;
            CommonFunc::PrintColorText(szLog, (int32_t)LogLevel::Error);
            CheckAndCreate();
            m_WriteMutex.unlock();

            return;
        }

        void LogInfo(const char* lpszFormat, ...)
        {
            if (m_LogLevel < LogLevel::Info)
            {
                return;
            }

            if (m_pLogFile == NULL)
            {
                return;
            }

            CHAR szLog[2048] = { 0 };

            tm CurTime = CommonFunc::GetCurrTmTime();
            snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][I] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

            va_list argList;
            va_start(argList, lpszFormat);
            vsnprintf(szLog + 28, 1024 - 28, lpszFormat, argList);
            va_end(argList);

            strncat_s(szLog, "\n", 3);

            m_WriteMutex.lock();
            fputs(szLog, m_pLogFile);
            fflush(m_pLogFile);
            m_LogCount++;
            CommonFunc::PrintColorText(szLog, (int32_t)LogLevel::Info);
            CheckAndCreate();
            m_WriteMutex.unlock();

            return;
        }

        void SetLogLevel(int Level)
        {
            m_LogLevel = (LogLevel)Level;

            return;
        }

        void SetTitle(const char* lpszFormat, ...)
        {
#ifdef WIN32
            char szLog[512] = { 0 };
            snprintf(szLog, 512, "%s: ", m_strPrefix.c_str());

            int32_t nSize = (int32_t)strlen(szLog);

            va_list argList;
            va_start(argList, lpszFormat);
            vsnprintf(szLog + nSize, 512 - nSize, lpszFormat, argList);
            va_end(argList);

            SetConsoleTitleA(szLog);
#endif
            return;
        }

        void CheckAndCreate()
        {
            //10M가 넘는 경우 새 파일 만들기
            if (ftell(m_pLogFile) < 1024 * 1024 * 10)
            {
                return;
            }

            fclose(m_pLogFile);

            m_pLogFile = NULL;

            tm CurTime = CommonFunc::GetCurrTmTime();

            CHAR szFileName[512];

            snprintf(szFileName, 512, "%s/%s-%02d%02d%02d-%02d%02d%02d.log", m_strLogDir.c_str(), m_strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);

            auto error = fopen_s(&m_pLogFile, szFileName, "w+");
            if (error != 0)
            {
                return;
            }

            m_LogCount = 0;
            return;
        }



    protected:
        std::mutex          m_WriteMutex;

        int32_t               m_LogCount;

        FILE* m_pLogFile;

        LogLevel               m_LogLevel;

        std::string         m_strPrefix;
        std::string         m_strLogDir;
    };

#define LOG_FUNCTION_LINE  CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);

#define ERROR_RETURN_true(P) \
    if((P) == false)\
    {\
        CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return true;    \
    }


#define ERROR_RETURN_false(P) \
    if((P) == false)\
    {\
        CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return false;   \
    }

#define ERROR_RETURN_NULL(P) \
    if((P) == false)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return NULL;    \
}

#define ERROR_RETURN_NONE(P) \
    if((P) == false)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return ;    \
}

#define ERROR_RETURN_VALUE(P, V) \
    if((P) == false)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return V;   \
}

#define ERROR_CONTINUE_EX(P) \
    if((P) == false)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#define ERROR_TO_CONTINUE(P) \
    if((P) == false)\
{\
    CLog::GetInstancePtr()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#define PARSE_FROM_PACKET(TYPE, VALUE)   TYPE VALUE; \
ERROR_RETURN_true(VALUE.ParsePartialFromArray(pNetPacket->m_pDataBuffer->GetData(), pNetPacket->m_pDataBuffer->GetBodyLenth()));

}