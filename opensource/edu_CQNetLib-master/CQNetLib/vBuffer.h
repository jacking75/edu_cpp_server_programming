#pragma once

#include "commonDef.h"
#include "singleton.h"


//TODO 유닛테스트 만들기
namespace CQNetLib
{	
	class VBufferImpl
	{
		const INT32 FLOAT_SIZE = sizeof(float);
		const INT32 DOUBLE_SIZE = sizeof(double);

	public:
		VBufferImpl(INT32 nMaxBufSize = 1024 * 100)
		{
			m_pszVBuffer = new char[nMaxBufSize];
			m_nMaxBufSize = nMaxBufSize;
			Init();
		}

		~VBufferImpl(void)
		{
			if (nullptr != m_pszVBuffer)
				delete[] m_pszVBuffer;
		}

		void GetChar(char& cCh)
		{
			cCh = (unsigned char)* m_pCurMark;
			m_pCurMark += 1;
			m_nCurBufSize += 1;
		}

		void GetShort(short& sNum)
		{
			sNum = (unsigned char)* m_pCurMark +
				(((unsigned char) * (m_pCurMark + 1)) << 8);
			m_pCurMark += 2;
			m_nCurBufSize += 2;
		}

		void GetInteger(INT32 & nNum)
		{
			nNum = (unsigned char)m_pCurMark[0] +
				((unsigned char)m_pCurMark[1] << 8) +
				((unsigned char)m_pCurMark[2] << 16) +
				((unsigned char)m_pCurMark[3] << 24);
			m_pCurMark += 4;
			m_nCurBufSize += 4;
		}
		
		void GetInteger64(INT64& n64Num) { GetStream((char*)& n64Num, sizeof(INT64)); }
		
		void GetUInteger64(UINT64& n64Num) { GetStream((char*)& n64Num, sizeof(UINT64)); }
		
		void GetString(char* pszBuffer)
		{
			short sLength;
			GetShort(sLength);
			if (sLength < 0 || sLength > MAX_PBUFSIZE)
			{
				return;
			}

			strncpy_s(pszBuffer, sLength - 1, m_pCurMark, sLength);
			*(pszBuffer + sLength) = NULL;
			m_pCurMark += sLength;
			m_nCurBufSize += sLength;
		}
		
		void GetStream(char* pszBuffer, short sLen)
		{
			if (sLen < 0 || sLen > MAX_PBUFSIZE)
			{
				return;
			}

			CopyMemory(pszBuffer, m_pCurMark, sLen);
			m_pCurMark += sLen;
			m_nCurBufSize += sLen;
		}
		
		void GetFloat(float& fNum)
		{
			CopyMemory(&fNum, m_pCurMark, FLOAT_SIZE);
			m_pCurMark += FLOAT_SIZE;
			m_nCurBufSize += FLOAT_SIZE;
		}
		
		void GetDouble(double& dNum)
		{
			CopyMemory(&dNum, m_pCurMark, DOUBLE_SIZE);
			m_pCurMark += DOUBLE_SIZE;
			m_nCurBufSize += DOUBLE_SIZE;
		}
		
		void SetInteger(INT32 nI)
		{
			*m_pCurMark++ = (char)nI;
			*m_pCurMark++ = (char)nI >> 8;
			*m_pCurMark++ = (char)nI >> 16;
			*m_pCurMark++ = nI >> 24;

			m_nCurBufSize += 4;
		}
		
		void SetInteger64(INT64 n64Num) { SetStream((char*)& n64Num, sizeof(INT64)); }
		
		void SetUInteger64(UINT64 n64Num) { SetStream((char*)& n64Num, sizeof(UINT64)); }
		
		void SetShort(short sShort)
		{
			*m_pCurMark++ = (char)sShort; //TODO 유닛테스트로 확인하기
			*m_pCurMark++ = sShort >> 8;
			m_nCurBufSize += 2;
		}
		
		void SetChar(char cCh)
		{
			*m_pCurMark++ = cCh;
			m_nCurBufSize += 1;
		}
		
		void SetString(char* pszBuffer)
		{
			short sLen = (short)strlen(pszBuffer);
			if (sLen < 0 || sLen > MAX_PBUFSIZE)
				return;
			SetShort(sLen);

			CopyMemory(m_pCurMark, pszBuffer, sLen);
			m_pCurMark += sLen;
			m_nCurBufSize += sLen;
		}
		
		//문자열말고 다른 byte stream을 넣을때 쓰인다
		void SetStream(char* pszBuffer, short sLen)
		{
			CopyMemory(m_pCurMark, pszBuffer, sLen);
			m_pCurMark += sLen;
			m_nCurBufSize += sLen;
		}
		
		void SetFloat(float fNum)
		{
			CopyMemory(m_pCurMark, &fNum, FLOAT_SIZE);
			m_pCurMark += FLOAT_SIZE;
			m_nCurBufSize += FLOAT_SIZE;
		}
		
		void SetDouble(double dNum)
		{
			CopyMemory(m_pCurMark, &dNum, DOUBLE_SIZE);
			m_pCurMark += DOUBLE_SIZE;
			m_nCurBufSize += DOUBLE_SIZE;
		}

		void SetBuffer(char* pVBuffer)
		{
			m_pCurMark = pVBuffer + PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH;
			m_nCurBufSize = PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH;
		}

		inline INT32 	GetMaxBufSize() { return m_nMaxBufSize; }
		inline INT32 	GetCurBufSize() { return m_nCurBufSize; }
		inline char*	GetCurMark() { return m_pCurMark; }
		inline char*    GetBeginMark() { return m_pszVBuffer; }

		bool CopyBuffer(char* pDestBuffer)
		{
			CopyMemory(m_pszVBuffer, (char*)& m_nCurBufSize
				, PACKET_SIZE_LENGTH);
			CopyMemory(pDestBuffer, m_pszVBuffer, m_nCurBufSize);
			return true;
		}
		
		void Init()
		{
			//PACKET_SIZE_LENGTH는 저절로 들어가게 만든다.
			m_pCurMark = m_pszVBuffer + PACKET_SIZE_LENGTH;
			m_nCurBufSize = PACKET_SIZE_LENGTH;
		}

	private:

		char* m_pszVBuffer;		//실제 버퍼
		char* m_pCurMark;		//현재 버퍼 위치

		INT32 	m_nMaxBufSize;		//최대 버퍼 사이즈
		INT32 m_nCurBufSize;		//현재 사용된 버퍼 사이즈

		VBufferImpl(const VBufferImpl &rhs);
		VBufferImpl &operator=(const VBufferImpl &rhs);
	};


	/*-------------------------------------------------------------------------------------------------*/
	//cVBufferImpl 클래스를 단일체로 만들기 위해.
	//cVBufferImpl 클래스를 직접적으로 단일체로 만들지 않은 까닭은 단일체 용도로 쓰지 않을때도 있기 때문
	/*-------------------------------------------------------------------------------------------------*/
	class VBuffer : public VBufferImpl, public Singleton< VBuffer >
	{
	public:
		VBuffer(void) { }

		virtual ~VBuffer(void) 
		{ 
		}
	};


#define g_pVBuffer	VBuffer::GetSingleton()
}


