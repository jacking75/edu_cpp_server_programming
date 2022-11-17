#include "stdafx.h"
#include <iostream>
#include "AdoManager.h"


/*
< TABLE >
CREATE TABLE [dbo].[Test_Temp2](
	[ID]		[nvarchar](50)	NOT NULL,
	[UserCode]	[int]			NOT NULL,
	[Lv]		[int]			NOT NULL,
	[Money]		[bigint]		NOT NULL
) ON [PRIMARY]

< SP >
CREATE PROCEDURE [dbo].[Test_Temp2_SelectUser] 
	@UserCode int,
	@TempValue bigint output
AS
BEGIN
	SET NOCOUNT ON;

    SELECT ID, Lv FROM Test_Temp2 WHERE UserCode=@UserCode 
    SET @TempValue = 234556
    
    RETURN 7777 
END
*/


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	asyncadodblib::DBConfig config;
	config.Setting( L"gunz2db\\gunz2_db", 
						L"dev", 
						L"dev", 
						L"G2_GAMEDB", 
						3, 
						true, 
						3 
					);

	asyncadodblib::DBManager* pDBmanager = new asyncadodblib::DBManager( config );

	asyncadodblib::AdoDB* pAdo = nullptr;
	asyncadodblib::CScopedAdo scopedado( pAdo, pDBmanager, true );

	
	INT32 nReturn = 0;
	INT32 nUserCode = 2;
	INT64 nTempValue = 0;
		
	pAdo->CreateParameter( L"RETURN", adInteger, adParamReturnValue, nReturn );
	pAdo->CreateParameter( L"UserCode", adInteger, adParamInput, nUserCode );
	pAdo->CreateParameter( L"TempValue", adBigInt, adParamInputOutput, nTempValue );
	
	
	pAdo->SetQuery(L"Test_Temp2_SelectUser");
	pAdo->Execute();

	if( !pAdo->IsSuccess() )
	{
		std::wcout << L"Test_Temp2_SelectUser SP ½ÇÆÐ" << std::endl;
		return 0;
	}

	WCHAR szID[16] = {0,};
	INT32 nLevel = 0;

	while( pAdo->GetEndOfFile() == false )
	{ 
		ISFAIL( pAdo->GetFieldValue(L"ID", szID, 16 ) );
		ISFAIL( pAdo->GetFieldValue(L"Lv", nLevel ) );

		std::wcout << L"ID : " << szID << ", Level : " << nLevel << std::endl;

		pAdo->MoveNext();
	}
	
	pAdo->GetParameter(L"RETURN", nReturn );
	pAdo->GetParameter(L"TempValue", nTempValue );

	std::wcout << L"RETURN : " << nReturn << ", TempValue : " << nTempValue << std::endl;


	delete pDBmanager;
	
	getchar();

	return 0;
}