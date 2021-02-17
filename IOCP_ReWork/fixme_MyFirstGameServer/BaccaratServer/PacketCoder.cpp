// PacketCoder.cpp: implementation of the CPacketCoder class.
//
//////////////////////////////////////////////////////////////////////

//#include <windows.h>
#include "define.h"
#include "PacketCoder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacketCoder::CPacketCoder()
{

}

CPacketCoder::~CPacketCoder()
{

}

void CPacketCoder::SetBuf(char *cpBuf)
{
	m_cpCoderBegin = cpBuf;
	m_cpCoderEnd   = cpBuf + HEADERSIZE;
}

int CPacketCoder::SetHeader(short sType)
{
	short		iSize;

	iSize = static_cast<short>( m_cpCoderEnd - m_cpCoderBegin - HEADERSIZE );
	CopyMemory( m_cpCoderBegin, &iSize, sizeof(short) );
	CopyMemory( m_cpCoderBegin+sizeof(short), &sType, sizeof(short) );

	return iSize + HEADERSIZE;
}

void CPacketCoder::GetHeader(short *sBodySize, short *sType)
{
	CopyMemory( sBodySize, m_cpCoderBegin, sizeof(short) );
	CopyMemory( sType, m_cpCoderBegin + sizeof(short), sizeof(short) );
}

void CPacketCoder::PutChar(char cData)
{
	*m_cpCoderEnd = cData;
	++m_cpCoderEnd;
}

void CPacketCoder::GetChar(char *cpData)
{
	*cpData = *m_cpCoderEnd;
	m_cpCoderEnd++;
}

void CPacketCoder::PutShort(short sData)
{
	CopyMemory( m_cpCoderEnd, &sData, sizeof(short) );
	m_cpCoderEnd += sizeof(short);
}

void CPacketCoder::GetShort(short *sData)
{
	CopyMemory( sData, m_cpCoderEnd, sizeof(short) );
	m_cpCoderEnd += sizeof(short);
}

void CPacketCoder::PutInt(int iData)
{
	CopyMemory( m_cpCoderEnd, &iData, sizeof(int) );
	m_cpCoderEnd += sizeof(int);
}

void CPacketCoder::GetInt(int *iData)
{
	CopyMemory( iData, m_cpCoderEnd, sizeof(int) );
	m_cpCoderEnd += sizeof(int);
}

void CPacketCoder::PutInt64(__int64 i64Data)
{
	CopyMemory( m_cpCoderEnd, &i64Data, sizeof(__int64) );
	m_cpCoderEnd += sizeof(__int64);
}

void CPacketCoder::GetInt64(__int64 *i64Data)
{
	CopyMemory( i64Data, m_cpCoderEnd, sizeof(__int64) );
	m_cpCoderEnd += sizeof(__int64);
}

void CPacketCoder::PutText(char *cpData, int iSize)
{
	CopyMemory( m_cpCoderEnd, cpData, iSize );
	m_cpCoderEnd += iSize;
}

void CPacketCoder::GetText(char *cpData, int iSize)
{
	CopyMemory( cpData, m_cpCoderEnd, iSize );
	m_cpCoderEnd += iSize;
}


void CPacketCoder::PutFloat(float fData)
{
	CopyMemory( m_cpCoderEnd, &fData, sizeof(float) );
	m_cpCoderEnd += sizeof(float);
}

void CPacketCoder::GetFloat(float *fData)
{
	CopyMemory( fData, m_cpCoderEnd, sizeof(float) );
	m_cpCoderEnd += sizeof(float);
}
