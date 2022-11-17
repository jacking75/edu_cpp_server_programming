#ifndef __datatypeconverter_h__
#define __datatypeconverter_h__

#pragma once

//#include <stdlib.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_stdlib.h>

namespace HalfNetwork
{

	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void ToNumeric(const ACE_TCHAR* text, T& numeric) 
	{}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, tstring& numeric) 
	{ 
		numeric = text; 
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, int& numeric) 
	{ 
		numeric = ACE_OS::atoi(text); 
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, char& numeric) 
	{ 
		numeric = ACE_OS::atoi(text); 
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, unsigned char& numeric) 
	{ 
		numeric = ACE_OS::atoi(text);	
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, short& numeric) 
	{ 
		numeric = ACE_OS::atoi(text); 
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, unsigned short& numeric) 
	{ 
		numeric = ACE_OS::atoi(text);	
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, uint32& numeric) 
	{ 
		numeric = ACE_OS::strtoul(text, NULL, 10);	
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, float& numeric)
	{ 
		numeric = (float)ACE_OS::atof(text); 
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, double& numeric)
	{ 
		numeric = ACE_OS::atof(text); 
	}

	template<>
	inline void ToNumeric(const ACE_TCHAR* text, bool& numeric) 
	{ 
		numeric = (ACE_OS::strcmp(text, ACE_TEXT("true"))==0);
	}


	//////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void ToString(const T numeric, ACE_TCHAR* str, uint32 buffer_size)
	{}

	template<>
	inline void ToString(const tstring numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{ 
		ACE_OS::sprintf(str, ACE_TEXT("%s"), numeric.c_str());
	}

	template<>
	inline void ToString(const char* numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{ 
		ACE_OS::sprintf(str, ACE_TEXT("%s"), numeric);
	}

	template<>
	inline void ToString(const int numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%d"), numeric);
	}

	template<>
	inline void ToString(const char numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%d"), numeric);
	}

	template<>
	inline void ToString(const unsigned char numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%d"), numeric);
	}

	template<>
	inline void ToString(const short numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%d"), numeric);
	}

	template<>
	inline void ToString(const unsigned short numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%d"), numeric);
	}

	template<>
	inline void ToString(const uint32 numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%d"), numeric);
	}

	template<>
	inline void ToString(const float numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%.4f"), numeric);
	}

	template<>
	inline void ToString(const double numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		ACE_OS::sprintf(str, ACE_TEXT("%.4f"), numeric);
	}

	template<>
	inline void ToString(const bool numeric, ACE_TCHAR* str, uint32 buffer_size) 
	{
		if (numeric)
			ACE_OS::sprintf(str, ACE_TEXT("true"));
		else
			ACE_OS::sprintf(str, ACE_TEXT("false"));
	}
} // namespace HalfNetwork


#endif // __datatypeconverter_h__