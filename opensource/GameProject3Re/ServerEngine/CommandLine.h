#pragma once

#include "CommonConvert.h"

#include <cstdint>
#include <string>
#include <map>


namespace hbServerEngine
{
class CommandLine
{
public:
	CommandLine(int32_t argc, char* argv[])
	{
		for (int i = 0; i < argc; i++)
		{
			char* pChar = strchr(argv[i], '=');
			if (pChar == nullptr)
			{
				continue;
			}

			std::string strName;
			strName.assign(argv[i], pChar - argv[i]);
			std::string strValue = pChar + 1;

			CommonConvert::StringTrim(strName);
			CommonConvert::StringTrim(strValue);

			m_ParameterMap.insert(std::make_pair(strName, strValue));
		}
	}

	~CommandLine()
	{
		m_ParameterMap.clear();
	}


	int32_t GetIntValue(const std::string& strKey) const
	{
		auto itor = m_ParameterMap.find(strKey);
		if (itor != m_ParameterMap.end())
		{
			return CommonConvert::StringToInt(itor->second.c_str());
		}

		return 0;
	}

	std::string GetStrValue(const std::string& strKey) const
	{
		auto it = m_ParameterMap.find(strKey);
		if (it != m_ParameterMap.end())
		{
			return it->second;
		}

		return "";
	}

	int64_t GetLongValue(const std::string& strKey) const
	{
		auto it = m_ParameterMap.find(strKey);
		if (it != m_ParameterMap.end())
		{
			return CommonConvert::StringToInt64(it->second.c_str());
		}

		return 0;
	}

	float GetFloatValue(const std::string& strKey) const
	{
		auto it = m_ParameterMap.find(strKey);
		if (it != m_ParameterMap.end())
		{
			return CommonConvert::StringToFloat(it->second.c_str());
		}

		return 0.0f;
	}



protected:
	std::map<std::string, std::string> m_ParameterMap;
};
}


