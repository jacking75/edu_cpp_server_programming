// ADOLib.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include "AdoManager.h"


void SetDBSetting(SAdoConfig& adoconfig )
{
	adoconfig.SetIP(_T("localhost\\TEST"));   //디비주소
	adoconfig.SetUserID(_T("sa"));			//아이디
	adoconfig.SetPassword(_T("dev"));			//패스워드
	adoconfig.SetInitCatalog(_T("TEST"));     //디비명
	adoconfig.SetConnectionTimeout(3);
	adoconfig.SetRetryConnection(true);
	adoconfig.SetMaxConnectionPool(3);
}

// 일반 쿼리문 - insert - 2
int _tmain(int argc, _TCHAR* argv[])
{  
	setlocale(LC_ALL, "");
	SAdoConfig adoconfig;
	SetDBSetting( adoconfig );

	CAdoManager* pAdomanager = new CAdoManager(adoconfig);
	CAdo* pAdo = NULL;
	{
		CScopedAdo scopedado(pAdo, pAdomanager, true);

		pAdo->SetQuery( _T("Insert Into Users Values( 'jacking3', '1111' )") );
		pAdo->Execute(adCmdText);
		if( !pAdo->IsSuccess() ) 
		{
			std::wcout << L"쿼리문 실패" << std::endl;
			return 0;
		}
		else
		{
			std::wcout << L"쿼리문 성공" << std::endl;
		}
		pAdo->SetCommit(true);
	}
	delete pAdomanager;
	
	getchar();
	return 0;
}

// 일반 쿼리문 - insert - 1
//int _tmain(int argc, _TCHAR* argv[])
//{  
//	setlocale(LC_ALL, "");
//	SAdoConfig adoconfig;
//	SetDBSetting( adoconfig );
//
//	CAdoManager* pAdomanager = new CAdoManager(adoconfig);
//	CAdo* pAdo = NULL;
//	{
//		CScopedAdo scopedado(pAdo, pAdomanager, false);
//
//		pAdo->SetQuery( _T("Insert Into Users Values( 'jacking2', '1111' )") );
//		pAdo->Execute(adCmdText);
//		if( !pAdo->IsSuccess() ) 
//		{
//			std::wcout << L"쿼리문 실패" << std::endl;
//			return 0;
//		}
//		else
//		{
//			std::wcout << L"쿼리문 성공" << std::endl;
//		}
//	}
//	delete pAdomanager;
//	
//	getchar();
//	return 0;
//}

// 일반 쿼리문 - select
//int _tmain(int argc, _TCHAR* argv[])
//{  
//	setlocale(LC_ALL, "");
//	SAdoConfig adoconfig;
//	SetDBSetting( adoconfig );
//
//	CAdoManager* pAdomanager = new CAdoManager(adoconfig);
//	CAdo* pAdo = NULL;
//	{
//		CScopedAdo scopedado(pAdo, pAdomanager, false);
//
//		pAdo->SetQuery(_T("SELECT UID, PWD FROM Users WHERE ID='jacking'"));
//		pAdo->Execute(adCmdText);
//		if(!pAdo->IsSuccess())
//		{
//			std::wcout << L"쿼리문 실패" << std::endl;
//			return 0;
//		}
//
//		int nUID = 0;
//		WCHAR szPWD[16];
//
//		if(!pAdo->GetEndOfFile() )
//		{
//			pAdo->GetFieldValue(_T("UID"), nUID);
//			pAdo->GetFieldValue(_T("PWD"), szPWD, 16);
//		}
//		else
//		{
//			std::wcout << L"jacking은 없습니다" << std::endl;
//			return 0;
//		}
//
//		std::wcout << L"UID : " << nUID << std::endl;
//		std::wcout << L"PWD : " << szPWD << std::endl;
//	}
//	delete pAdomanager;
//	
//	getchar();
//	return 0;
//}
