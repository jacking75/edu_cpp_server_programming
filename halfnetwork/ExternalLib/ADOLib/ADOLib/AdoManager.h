#pragma once
#include "ado.h"
#include "Lock.h"

/** 
\brief		커넥션풀 ADO객체를 생성하여 stack에 관리한다.
\author		김영찬
*/
class CAdoManager
{
	enum{MAX_ARRAY_SIZE=20};
public:
	explicit CAdoManager(SAdoConfig& adoconfig)
	{
		m_bSuccessConnection = true;
		int MaxConnectionPoolCount = adoconfig.GetMaxConnectionPool();

		_ASSERTE(MaxConnectionPoolCount <= MAX_ARRAY_SIZE);
		for(int i = 0; i < MaxConnectionPoolCount; ++i)
		{
			m_pAdoStack[i] = new CAdo(adoconfig);
			if( FALSE == m_pAdoStack[i]->Open() )
			{
				m_bSuccessConnection = false;
				break;
			}
		}

		m_nTopPos = m_nMaxAdo = MaxConnectionPoolCount - 1;
	}

	// 연결 성공 여부
	bool IsSuccessConnection() { return m_bSuccessConnection; 	}

	void PutAdo(CAdo* pAdo)
	{
		ScopedLock lock(m_Lock);
		_ASSERTE(m_nTopPos < m_nMaxAdo);
		m_pAdoStack[++m_nTopPos] = pAdo;
		return;
	}

	CAdo* GetAdo()
	{
		ScopedLock lock(m_Lock);
		_ASSERTE(m_nTopPos >= 0);
		return m_pAdoStack[m_nTopPos--];
	}
private:
	int m_nTopPos;
	int m_nMaxAdo;
	bool m_bSuccessConnection;	 // 연결 성공 여부
	
	CAdo* m_pAdoStack[MAX_ARRAY_SIZE];
	CriticalSection m_Lock;
};

/** 
\brief		객체 생성시 커넥션풀로부터 ADO객체를 얻은 후 소멸시 ADO객체를 커넥션풀로 돌려준다.
\par		부가기능 명시적 트랜잭션
\author		김영찬
*/
class CScopedAdo
{
public:
	explicit CScopedAdo(CAdo* &pAdo, CAdoManager* pAdoManager, bool bAutoCommit = false)
		:m_pAdoManager(pAdoManager)
	{
		m_pAdo = pAdoManager->GetAdo();
		pAdo = m_pAdo;
		pAdo->SetAutoCommit(bAutoCommit);

		if(true == bAutoCommit)
			pAdo->BeginTransaction();
	}

	~CScopedAdo()
	{
		if(true == m_pAdo->GetAutoCommit())
		{
			if(m_pAdo->GetSuccess())
				m_pAdo->CommitTransaction();
			else
				m_pAdo->RollbackTransaction();
		}

		m_pAdo->Init();
		m_pAdo->Release();
		m_pAdoManager->PutAdo(m_pAdo);
	}

private:
	CAdoManager* m_pAdoManager;
	CAdo* m_pAdo;
};
