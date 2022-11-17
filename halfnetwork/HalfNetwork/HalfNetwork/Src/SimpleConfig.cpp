#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "SimpleConfig.h"

namespace HalfNetwork
{

	SimpleConfig::SimpleConfig(void)
	{
	}

	SimpleConfig::SimpleConfig(const SimpleConfig& other)
	{
		m_container.clear();
		StringMapConstIterator endIter = other.m_container.end();
		for(StringMapConstIterator iter = other.m_container.begin(); iter != endIter; ++iter)
			Add(iter->first, iter->second);
	}

	SimpleConfig::~SimpleConfig(void)
	{
	}

	bool SimpleConfig::AddImpl(const tstring& key, const tstring& value)
	{
		std::pair<StringMapIterator, bool> result;
		result = m_container.insert(std::pair<tstring, tstring>(key, value));
		if (false == result.second)
			return false;
		return true;
	}

	bool SimpleConfig::Remove(const tstring& key)
	{
		StringMapIterator iter = m_container.find(key);
		if (iter == m_container.end())
			return false;
		m_container.erase(iter);
		return true;
	}

	bool SimpleConfig::ReadFile( const ACE_TCHAR* filename )
	{
		FILE* r = fopen(ACE_TEXT_ALWAYS_CHAR(filename), "rb");
		if (NULL == r)
			return false;

		uint32 fileBufferSize = 512;
		size_t readFileSize = 0;
		ACE_TCHAR* fileBuffer = NULL;
		do
		{
			fileBufferSize *= 2;
			if (NULL != fileBuffer)
			{
				delete [] fileBuffer;
				fileBuffer = NULL;
			}
			fileBuffer = new ACE_TCHAR[fileBufferSize];
			fseek(r , 0, SEEK_SET);
			readFileSize = fread(fileBuffer, sizeof(ACE_TCHAR), fileBufferSize, r);
		} while(readFileSize >= fileBufferSize);

		fclose(r);
		tstring readString(fileBuffer, readFileSize);
		delete [] fileBuffer;
		while ( readString.find(ACE_TEXT("\r\n")) != tstring::npos )
			readString.erase(readString.find (ACE_TEXT("\r\n")), 2 );

		ACE_TCHAR unicodeBom = (ACE_TCHAR)0xFEFF;
		while ( readString.find(unicodeBom) != tstring::npos )
			readString.erase(readString.find (unicodeBom), 1 );

		return Parse(readString);
	}

	bool SimpleConfig::Parse(const tstring& source)
	{
		if (-1 == (int)source.find(ACE_TEXT("=")) || -1 == (int)source.find(ACE_TEXT(";")))
			return false;

		tstring subSource = source;
		tstring key, value;

		while ( subSource.find(ACE_TEXT(" ")) != tstring::npos )
			subSource.erase(subSource.find (ACE_TEXT(" ")), 1);

		while(subSource.length() > 0)
		{
			int keyPos = (int)subSource.find(ACE_TEXT("="));
			if (keyPos<0)
				break;	

			key = subSource.substr(0, keyPos);
			subSource = subSource.substr(keyPos+1);
			int valuePos = (int)subSource.find(ACE_TEXT(";"));
			if (valuePos<0)
				break;
			value = subSource.substr(0, valuePos);
			Add(key, value);
			subSource = subSource.substr(valuePos+1);
		}
		return true;
	}

	tstring SimpleConfig::GetValueImpl(const tstring& key) const
	{
		StringMapConstIterator iter = m_container.find(key);
		if (iter != m_container.end())
			return iter->second;
		return ACE_TEXT("");
	}

	void SimpleConfig::Clear()
	{
		m_container.clear();
	}

	tstring SimpleConfig::MakeString() const
	{
		tstring result;
		StringMapReverseIterator endIter = m_container.rend();
		for(StringMapReverseIterator iter = m_container.rbegin(); iter != endIter; ++iter)
			result += iter->first + ACE_TEXT("=") + iter->second + ACE_TEXT(";");
		return result;
	}
} // namespace HalfNetwork