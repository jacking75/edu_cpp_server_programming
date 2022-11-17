#ifndef __echobackchecker_h__
#define __echobackchecker_h__

#pragma once

#include <map>

struct VerrifyData
{
	unsigned int streamId;
	unsigned int length;
	unsigned int tick;
};

class EchoBackChecker
{
public:
	void		AddSendBuffer(unsigned int streamId, const char* buffer, unsigned int length);
	bool		IsValidBuffer(unsigned int streamId, const char* buffer, unsigned int length, int& elaspedTick);
	unsigned int	GetElementCount();

protected:
	unsigned int	GetHashCode(const char* buffer, unsigned int length );
	void		InsertToMap(unsigned int key, unsigned int streamId, unsigned int length);
	bool		VerifyKey(unsigned int key, unsigned int streamId, unsigned int length, int& elaspedTick);

private:
	typedef std::map<unsigned int, VerrifyData>	KeyMap;
	typedef	KeyMap::iterator							KeyMapIterator;
	KeyMap	_keyMap;
};


#endif // __echobackchecker_h__
