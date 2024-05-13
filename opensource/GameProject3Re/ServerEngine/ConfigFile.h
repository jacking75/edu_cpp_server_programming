#pragma once

#include "CommonConvert.h"

#include <string>
#include <map>


namespace hbServerEngine
{

class ConfigFile
{
private:
	ConfigFile() noexcept = default;
	~ConfigFile() = default;

public:
	static ConfigFile* GetInstancePtr() noexcept 
    {
        static ConfigFile ConfigFile;

        return &ConfigFile;
    }

	bool Load(std::string strFileName)
    {
        FILE* pFile = nullptr; 
        
        if (fopen_s(&pFile, strFileName.c_str(), "r+") != 0)
        {
            return false;
        }

        char szBuff[256] = { 0 };

        do
        {
            fgets(szBuff, 256, pFile);

            if (szBuff[0] == '#')
            {
                continue;
            }

            char* pChar = strchr(szBuff, '=');
            if (pChar == NULL)
            {
                continue;
            }

            std::string strName;
            strName.assign(szBuff, pChar - szBuff);
            std::string strValue = pChar + 1;
            CommonConvert::StringTrim(strName);
            CommonConvert::StringTrim(strValue);

            m_Values.insert(std::make_pair(strName, strValue));

        } while (!feof(pFile));

        fclose(pFile);

        return true;
    }

	std::string GetStringValue(std::string strName)
    {
        std::map<std::string, std::string>::iterator itor = m_Values.find(strName);
        if (itor != m_Values.end())
        {
            return itor->second;
        }

        return "";
    }

	int32_t GetIntValue( std::string VarName)
    {
        return atoi(GetStringValue(VarName).c_str());
    }

	float GetFloatValue( std::string VarName)
    {
        return (float)atof(GetStringValue(VarName).c_str());
    }

	double GetDoubleValue( std::string VarName)
    {
        return atof(GetStringValue(VarName).c_str());
    }

	int32_t  GetRealNetPort(std::string VarName)
    {
        int32_t nPort = GetIntValue(VarName);
        if (nPort > 0)
        {
            return nPort;
        }

        if (VarName == "logic_svr_port")
        {
            return GetIntValue("areaid") + 10000;
        }
        else if (VarName == "db_svr_port")
        {
            return GetIntValue("areaid") + 20000;
        }
        else if (VarName == "proxy_svr_port")
        {
            return GetIntValue("areaid") + 30000;
        }
        else if (VarName == "log_svr_port")
        {
            return GetIntValue("areaid") + 40000;
        }
        return GetIntValue(VarName);
    }



private:
	std::map<std::string, std::string> m_Values;
};

}