#ifndef __simpleconfig_h__
#define __simpleconfig_h__

#pragma once

#include <string>
#include <map>
#include "DataTypeConvertor.h"

namespace HalfNetwork
{
	class SimpleConfig
	{
	public:
		typedef std::map<tstring, tstring>				StringKeyValueMap;
		typedef StringKeyValueMap::iterator								StringMapIterator;
		typedef StringKeyValueMap::const_iterator					StringMapConstIterator;
		typedef StringKeyValueMap::const_reverse_iterator StringMapReverseIterator;

	public:
		SimpleConfig();
		SimpleConfig(const SimpleConfig& other);
		virtual ~SimpleConfig();

		template<typename T>
		bool Add(const tstring& key, const T& value)
		{
			const int MaxValueLength = 64;
			ACE_TCHAR strValue[MaxValueLength] = {0,};
			ToString(value, strValue, MaxValueLength);
			return AddImpl(key, strValue);
		}

		bool Remove(const tstring& key);
		void Clear();

		template<typename T>
		T		GetValue(const tstring& key) const
		{
			tstring result = GetValueImpl(key);
			T value;
			ToNumeric(result.c_str(), value);
			return value;
		}

		////////////////////////////////////////////////////////////////////////
		// Description:
		//   Parsing and Add to container 
		//   i.e.) name=alice;level=40; => ["name", "alice"], ["level", "40"]
		////////////////////////////////////////////////////////////////////////
		bool ReadFile(const ACE_TCHAR* filename);
		bool Parse(const tstring& source);
		tstring MakeString() const;
		uint32 MakeBuffer(char* buffer, uint32 length) const;
		void EncryptKey(unsigned char* key, unsigned char keyLen);

	protected:
		bool AddImpl(const tstring& key, const tstring& value);
		tstring GetValueImpl(const tstring& key) const;

	private:
		StringKeyValueMap m_container;
	};


} // namespace HalfNetwork

#endif // __simpleconfig_h__