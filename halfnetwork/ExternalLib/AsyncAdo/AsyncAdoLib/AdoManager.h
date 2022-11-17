#pragma once
#include "ado.h"
#include "Lock.h"

/** 
\brief		커넥션풀 ADO객체를 생성하여 stack에 관리한다.
\author		김영찬
*/

// 주석 참고 문헌
// http://msdn.microsoft.com/ko-kr/library/5ast78ax(v=vs.80).aspx
// http://msdn.microsoft.com/ko-kr/library/z04awywx(v=vs.80)

namespace asyncadodblib
{
	class DBManager
	{
		enum{MAX_ARRAY_SIZE=20};
	public:
		explicit DBManager( DBConfig& dboconfig )
		{
			m_bSuccessConnection = true;
			int MaxConnectionPoolCount = dboconfig.GetMaxConnectionPool();

			_ASSERTE( MaxConnectionPoolCount <= MAX_ARRAY_SIZE );

			for( int i = 0; i < MaxConnectionPoolCount; ++i )
			{
				m_pAdoStack[i] = new AdoDB(dboconfig);

				if( m_pAdoStack[i]->Open() == false )
				{
					m_bSuccessConnection = false;
					break;
				}
			}

			m_nTopPos = m_nMaxAdo = MaxConnectionPoolCount - 1;
		}

		// 연결 성공 여부
		bool IsSuccessConnection() { return m_bSuccessConnection; }
	
		void PutDB( AdoDB* pAdo )
		{
			ScopedLock lock(m_Lock);
			
			_ASSERTE( m_nTopPos < m_nMaxAdo );

			m_pAdoStack[ ++m_nTopPos ] = pAdo;
			return;
		}

		AdoDB* GetDB()
		{
			ScopedLock lock(m_Lock);

			_ASSERTE( m_nTopPos >= 0 );
			
			return m_pAdoStack[ m_nTopPos-- ];
		}

	private:
		int m_nTopPos;
		int m_nMaxAdo;
		bool m_bSuccessConnection;	 // 연결 성공 여부
	
		AdoDB* m_pAdoStack[MAX_ARRAY_SIZE];
		CSSpinLockWin32 m_Lock;
	};

	/** 
	\brief		객체 생성시 커넥션풀로부터 ADO객체를 얻은 후 소멸시 ADO객체를 커넥션풀로 돌려준다.
	\par		부가기능 명시적 트랜잭션
	\author		김영찬
	*/
	class CScopedAdo
	{
	public:
		explicit CScopedAdo( AdoDB* &pAdo, DBManager* pAdoManager, bool bAutoCommit = false )
			:m_pAdoManager(pAdoManager)
		{
			m_pAdo = pAdoManager->GetDB();
			pAdo = m_pAdo;
			pAdo->SetAutoCommit( bAutoCommit );

			if( bAutoCommit == false ) 
			{ 
				pAdo->BeginTransaction(); 
			}
		}

		~CScopedAdo()
		{
			if( m_pAdo->CanAutoCommit() == false )
			{
				if( m_pAdo->CanGetParamGetFiled() && m_pAdo->CanCommitTransaction() )
				{
					m_pAdo->CommitTransaction();
				}
				else
				{
					m_pAdo->RollbackTransaction();
				}
			}

			m_pAdo->Init();
			m_pAdo->Release();
			m_pAdoManager->PutDB( m_pAdo );
		}

	private:
		DBManager* m_pAdoManager;
		AdoDB* m_pAdo;
	};
}
