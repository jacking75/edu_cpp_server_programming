#include "SerialGenerator.h"
#include <time.h>


unsigned __int32 CSerialGenerator::GenerateSerialID()
{
	return m_currentSerialID.fetch_and_add(1);
}


CSerialGenerator::CSerialGenerator(unsigned __int32 init_)
	: m_currentSerialID(init_)
{
}


CSerialGenerator::~CSerialGenerator()
{
}



unsigned __int64 CExternalSerialGenerator::GenerateSerialID()
{
	unsigned short spinCount = m_currentNumber.fetch_and_add(1);
	__int64 externalSerialID = m_uniqueAppID | (static_cast<__int64>(spinCount) << 32) | ::_time32(NULL);
	return externalSerialID;
}


CExternalSerialGenerator::CExternalSerialGenerator(unsigned short uniqueAppId_)
	: m_uniqueAppID(__int64(uniqueAppId_) << 48),
	m_currentNumber(0)
{
}

CExternalSerialGenerator::~CExternalSerialGenerator()
{
}
