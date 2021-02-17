#pragma once

#include <tbb/atomic.h>

///////////////////////////////////////////////////////////////////////////////
// 서버가 살아있는 동안 해당 프로세스에 유니크한 시리얼 생성 클래스
// (시작 시리얼 아이디)
///////////////////////////////////////////////////////////////////////////////
class CSerialGenerator 
{
private:
	tbb::atomic<unsigned __int32> m_currentSerialID;   ///< 현재까지 발급된 시리얼 아이디

public:
	/// 시리얼을 발급한다
	unsigned __int32 GenerateSerialID();

public:
	CSerialGenerator(unsigned __int32 init_ = 1);
	~CSerialGenerator();
};


///////////////////////////////////////////////////////////////////////////////
// 전 서버에 유니크한 시리얼 생성 클래스
// (해당 프로세스의 유니크 아이디(서버아이디))
///////////////////////////////////////////////////////////////////////////////
class CExternalSerialGenerator 
{
private:
	tbb::atomic<unsigned short> m_currentNumber;   ///< 시리얼 조합용 카운트
	const __int64 m_uniqueAppID;                   ///< 해당 프로세스의 유니크한 아이디(서버아이디)


public:
	/// 시리얼을 생성한다.
	unsigned __int64 GenerateSerialID();


public:
	CExternalSerialGenerator(unsigned short uniqueAppId_ = 1);
	~CExternalSerialGenerator();
};

