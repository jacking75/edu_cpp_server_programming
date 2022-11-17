#include "stdafx.h"
#include "EchoBackChecker.h"
#include <ace/Message_Block.h>
#include "TimerUtil.h"


void EchoBackChecker::AddSendBuffer( unsigned int streamId, const char* buffer, unsigned int length )
{
	unsigned int hashKey = GetHashCode(buffer, length);
	InsertToMap(hashKey, streamId, length);
}

bool EchoBackChecker::IsValidBuffer( unsigned int streamId, const char* buffer, unsigned int length, int& elaspedTick )
{
	unsigned int hashKey = GetHashCode(buffer, length);
	return VerifyKey(hashKey, streamId, length, elaspedTick);
}

unsigned int EchoBackChecker::GetHashCode( const char* buffer, unsigned int length )
{
	unsigned int ch = 0;
	unsigned int result = 0;
	result = 5381;
	for( unsigned int i=0; i<length; i++ )
	{
		ch = (unsigned int)buffer[i];
		result = ((result<< 5) + result) + ch; // hash * 33 + ch
	}
	return result;
}

void EchoBackChecker::InsertToMap( unsigned int key, unsigned int streamId, unsigned int length)
{
	std::pair<KeyMapIterator,bool> result;
	VerrifyData value;
	value.length = length;
	value.streamId = streamId;
	value.tick = HalfNetwork::GetTick();
	result = _keyMap.insert(std::make_pair(key, value));
	if (false == result.second)
		printf("same key\n");
		//ACE_ASSERT(false);
}

bool EchoBackChecker::VerifyKey( unsigned int key, unsigned int streamId, unsigned int length, int& elaspedTick)
{
	KeyMapIterator iter = _keyMap.find(key);
	if (iter == _keyMap.end())
		return false;
	VerrifyData value = iter->second;
	_keyMap.erase(iter);
	elaspedTick = HalfNetwork::GetTick() - value.tick;
	return (streamId == value.streamId && length == value.length);
}

unsigned int EchoBackChecker::GetElementCount()
{
	return (unsigned int)_keyMap.size();
}