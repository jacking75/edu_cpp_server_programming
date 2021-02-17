#pragma once

#include "zlib.h"

#define	COMP_BUFSIZE		0x1000
#define	UNCOMP_BUFSIZE		0x8000


class CMemoryStreamReader
{
public:
	const BYTE*	m_pData;
	UINT		m_Size;
	UINT		m_Pos;

public:
	CMemoryStreamReader( const BYTE* pData, UINT nSize )
		: m_pData(pData), m_Size(nSize), m_Pos(0)
	{
	}

	~CMemoryStreamReader(void)
	{
	}

	BOOL Read( void* pv, ULONG cb, ULONG* pcbRead )
	{
		UINT nDiff = m_Size - m_Pos;
		UINT nMin  = min( nDiff, cb );
		
		memcpy( pv, m_pData + m_Pos, nMin );
		m_Pos += nMin;
		
		if( pcbRead )
		{
			*pcbRead = nMin;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
};



class CMemoryStreamWriter
{
public:
	BYTE*	m_pData;
	UINT		m_Size;
	UINT		m_Pos;

public:
	CMemoryStreamWriter( BYTE* pData, UINT nSize )
		: m_pData(pData), m_Size(nSize), m_Pos(0) 
	{
	}

	~CMemoryStreamWriter(void)
	{
	}

	BOOL Write( const void* pv, ULONG cb, ULONG* pcbWritten )
	{
		UINT nDiff = m_Size - m_Pos;
		
		if( nDiff < cb )
			return FALSE;

        
		memcpy( m_pData, pv, cb );
		m_Pos += cb;

		*pcbWritten = cb;

		return TRUE;
	}
};


class CZipCompress
{
public:
	CZipCompress(){}
	~CZipCompress(){}
	
	int Compress(BYTE* dst, UINT dstLen, const BYTE* src, UINT srcLen)
	{
		CMemoryStreamReader sr(src , srcLen);
		CMemoryStreamWriter sw(dst , dstLen);

		Deflate(&sw, &sr);

		return sw.m_Pos;
	}

	void Deflate( CMemoryStreamWriter* pOutput, CMemoryStreamReader* pInput)
	{
		z_stream zstm;
		memset(&zstm,0,sizeof(z_stream));

		BYTE zBufIn[UNCOMP_BUFSIZE];
		BYTE zBufOut[COMP_BUFSIZE];

		deflateInit(&zstm, Z_DEFAULT_COMPRESSION);

		int err = Z_OK;
		while ( TRUE )
		{
			ULONG cbRead = 0;
			BOOL bResult = pInput->Read(zBufIn, sizeof(zBufIn), &cbRead);
			if ( bResult == FALSE || cbRead == 0 )
				break;

			zstm.next_in = (Bytef*)zBufIn;
			zstm.avail_in = (uInt)cbRead;

			while ( TRUE )
			{
				zstm.next_out = (Bytef*)zBufOut;
				zstm.avail_out = sizeof(zBufOut);

				err = deflate(&zstm, Z_SYNC_FLUSH);
				if (err != Z_OK)
					break;

				ULONG cbWrite = sizeof(zBufOut) - zstm.avail_out;
				bResult = pOutput->Write(zBufOut, cbWrite, &cbWrite );
				if ( bResult == FALSE || cbWrite == 0 )
					break;

				if ( zstm.avail_out != 0 )
					break;
			}
		}

		err = deflateEnd(&zstm);
	}


	int UnCompress( BYTE* dst, UINT dstLen, const BYTE* src, UINT srcLen )
	{
		CMemoryStreamReader sr(src , srcLen);
		CMemoryStreamWriter sw(dst , dstLen);

		Inflate(&sw, &sr);

		return sw.m_Pos;
	}

	void Inflate(CMemoryStreamWriter* pOutput, CMemoryStreamReader* pInput)
	{
		z_stream zstm;
		memset(&zstm,0,sizeof(z_stream));

		BYTE zBufIn[COMP_BUFSIZE];
		BYTE zBufOut[UNCOMP_BUFSIZE];

		inflateInit(&zstm);

		int err = Z_OK;
		while ( TRUE )
		{
			ULONG cbRead = 0;
			BOOL bResult = pInput->Read(zBufIn, sizeof(zBufIn), &cbRead);
			if ( bResult == FALSE || cbRead == 0 )
				break;

			zstm.next_in = (Bytef*)zBufIn;
			zstm.avail_in = (uInt)cbRead;

			while ( TRUE )
			{
				zstm.next_out = (Bytef*)zBufOut;
				zstm.avail_out = sizeof(zBufOut);

				err = inflate(&zstm, Z_SYNC_FLUSH);
				if (err != Z_OK)
					break;

				ULONG cbWrite = sizeof(zBufOut) - zstm.avail_out;
				bResult = pOutput->Write(zBufOut, cbWrite, &cbWrite );
				if ( bResult == FALSE || cbWrite == 0 )
					break;

				if ( zstm.avail_out != 0 )
					break;
			}
		}

		err = inflateEnd(&zstm);
	}
};




