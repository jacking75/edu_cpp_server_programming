// PacketCoder.h: interface for the CPacketCoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKETCODER_H__934ED373_DC28_4020_9F53_A7FC556BF493__INCLUDED_)
#define AFX_PACKETCODER_H__934ED373_DC28_4020_9F53_A7FC556BF493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPacketCoder  
{

private:
	char		*m_cpCoderBegin, *m_cpCoderEnd;
	
public:
	void GetFloat( float* fData );
	void PutFloat( float fData );
	void GetInt64( __int64* i64Data );
	void PutInt64( __int64 i64Data );
	void GetText( char* cpData, int iSize );
	void PutText( char* cpData, int iSize );
	void GetInt( int* iData );
	void PutInt( int iData );
	void GetShort( short* sData );
	void PutShort( short sData );
	void GetChar( char* cpData );
	void PutChar( char cData );
	void GetHeader( short* sBodySize, short *sType );
	int SetHeader( short sType );
	void SetBuf( char* cpBuf );
	CPacketCoder();
	virtual ~CPacketCoder();

};

#endif // !defined(AFX_PACKETCODER_H__934ED373_DC28_4020_9F53_A7FC556BF493__INCLUDED_)
