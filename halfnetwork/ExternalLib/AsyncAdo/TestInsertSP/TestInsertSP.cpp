#include "stdafx.h"
#include <iostream>
#include "AdoManager.h"



/*
< TABLE >
CREATE TABLE dbo.[Test_Temp3] (
	aaa int,
	bbb nvarchar(30),
	ccc varbinary(50),
	ddd bit,
	eee tinyint,
	fff smallint,
	ggg bigint,
	hhh smalldatetime,
	iii datetime,
	jjj float,
	kkk image
) ON [PRIMARY]
GO


< SP >
create PROC Test_Temp3_Insert
    @arg_aaa            INT				OUTPUT,
    @arg_bbb            VARCHAR(30)		OUTPUT,
    @arg_ccc			VARBINARY(50)	OUTPUT,
    @arg_ddd		    BIT				OUTPUT,
    @arg_eee			TINYINT			OUTPUT,
    @arg_fff		    SMALLINT		OUTPUT,
    @arg_ggg		    BIGINT			OUTPUT,
    @arg_hhh		    SMALLDATETIME	OUTPUT,
    @arg_iii			DATETIME		OUTPUT,
    @arg_jjj			FLOAT			OUTPUT,
    @arg_kkk			IMAGE			OUTPUT
AS       
DECLARE  
    @v_Flag             BIT
BEGIN
	SET NOCOUNT ON
	SET XACT_ABORT ON

    INSERT INTO Test_Temp3 values(@arg_aaa, @arg_bbb, @arg_ccc, @arg_ddd, @arg_eee, @arg_fff, @arg_ggg, @arg_hhh, @arg_iii, @arg_jjj, @arg_kkk)
    
	SELECT top 1 aaa, '2', '3', '4' FROM Test_Temp3
	SELECT top 1 bbb FROM Test_Temp3
	SELECT top 1 ccc FROM Test_Temp3
	SELECT top 1 ddd FROM Test_Temp3
	SELECT top 1 eee FROM Test_Temp3
	SELECT top 1 fff FROM Test_Temp3
	SELECT top 1 ggg FROM Test_Temp3
	SELECT top 1 hhh FROM Test_Temp3
	SELECT top 1 iii FROM Test_Temp3    
	SELECT top 1 jjj FROM Test_Temp3
	SELECT top 1 kkk FROM Test_Temp3
		
	SET @arg_aaa = 777777777
	SET @arg_bbb = 'iiiiiiiiiiii'
	SELECT top 1 @arg_ccc = ccc from Test_Temp3
	SET @arg_ddd = 1
	SET @arg_eee = 55
	SET @arg_fff = 20000
	SET @arg_ggg = 9223372036854775800
	SET @arg_hhh = '20070101 23:23:11'
	SET @arg_iii = '20081201 23:23:11'
	SET @arg_jjj = 2384.2373245
		
	return 123464
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


	DWORD nParam = 1231415151, nRtnParam = 0;
	WCHAR szParam[30] = L"uuuuu";
	BYTE pbyParam[10000] = {0X01, 0X01, 0X00, 0X01, 0X06, 0X07,};
	BYTE pbyDATA[10000]={0X01, 0X01, 0X00, 0X01, 0X06, 0X07,};
	bool bBoolValue = 1;
	char byByteValue = 100;
	WORD wWordValue = 20000;
	int nReturn = 0;
	INT64 i64BigIntValue = 8223372036854775801;
	COleDateTime oleTime;
	float fValue = 74.234738123f;
	int nSize = 0;
	WCHAR szdsjo[4]={0,};
	_variant_t vValue(1000);
	BYTE pValue[1000]={0,};
	oleTime.SetDateTime(2008, 2, 28, 23, 59, 59);


	pAdo->CreateParameter(L"return",adInteger, adParamReturnValue, nReturn);
	pAdo->CreateParameter(L"@v_aaa",adInteger, adParamInputOutput, vValue);
	pAdo->CreateParameter(L"@v_bbb",adVarChar, adParamInputOutput, (TCHAR*)NULL, 1);
	pAdo->CreateParameter(L"@v_ccc",adVarBinary, adParamInputOutput, (BYTE*)NULL, 1);
	pAdo->CreateParameter(L"@v_ddd",adBoolean, adParamInputOutput, bBoolValue);
	pAdo->CreateParameter(L"@v_eee",adTinyInt, adParamInputOutput, byByteValue);
	pAdo->CreateParameter(L"@v_fff",adSmallInt, adParamInputOutput, wWordValue);
	pAdo->CreateParameter(L"@v_ggg",adBigInt, adParamInputOutput, i64BigIntValue);
	pAdo->CreateParameter(L"@v_hhh",adDBTimeStamp, adParamInputOutput, oleTime);
	pAdo->CreateParameter(L"@v_iii",adDBTimeStamp, adParamInputOutput, oleTime);
	pAdo->CreateParameter(L"@v_jjj",adDouble, adParamInputOutput, fValue);
	pAdo->CreateParameter(L"@v_kkk",adBinary, adParamInputOutput, (BYTE*)NULL, 1);

	wstring strString;
	int i = 5;

	while(i > 0)
	{
		strString += L"yy";
		oleTime.SetDateTime(2007, 12, 12+i, 23, 10+i, 1+i);
		pAdo->UpdateParameter<COleDateTime&>(L"@v_hhh", oleTime);
		pAdo->UpdateParameter(L"@v_hhh", oleTime);
		pAdo->UpdateParameter(L"@v_bbb", (WCHAR*)strString.c_str(), strString.size());
		pAdo->UpdateParameter(L"@v_kkk", pbyParam, 10);
		pAdo->UpdateParameter(L"@v_ccc", pbyParam, 10);
			
		if(!pAdo->IsSuccess()){ return 0;	}

		pAdo->SetQuery(L"Test_Temp3_Insert");

		pAdo->Execute();
		if(!pAdo->IsSuccess()){ return 0;	}

		nParam = 0;
		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"aaa", nParam));
			std::wcout << "aaa : " << nParam << std::endl;
			pAdo->MoveNext();
		}
		if(!pAdo->IsSuccess()){ return 0;}

		pAdo->NextRecordSet();
		if(!pAdo->IsSuccess()){ return 0;}

		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"bbb", szParam, 30));
			std::wcout << "bbb : " << szParam << std::endl;
			pAdo->MoveNext();
		}
		if(!pAdo->IsSuccess()){return 0;}


		pAdo->NextRecordSet();
		if(!pAdo->IsSuccess()){ return 0;}

		while(!pAdo->GetEndOfFile())
		{
			memset(pValue, 0x00, 50);
			ISFAIL(pAdo->GetFieldValue(L"ccc", pValue, 10, nSize));
			std::wcout << "Size : " << nSize;

			for(int i = 0; i < nSize; ++i) { 
				std::wcout << "bbb : " << pValue[i];
			}
			
			std::wcout << std::endl;
			pAdo->MoveNext();
		}

		if( !pAdo->IsSuccess() )
		{
			return 0;
		}

		pAdo->NextRecordSet();
		if(!pAdo->IsSuccess()){ return 0;}

		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"ddd", bBoolValue));
			std::wcout << "ddd : " << bBoolValue << std::endl;
			
			pAdo->MoveNext();
		}

		if( !pAdo->IsSuccess() ) 
		{ 
			return 0;
		}

		pAdo->NextRecordSet();
		
		if( !pAdo->IsSuccess() ) 
		{ 
			return 0;
		}

		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"eee", byByteValue));
			std::wcout << "eee : " << byByteValue << std::endl;
			
			pAdo->MoveNext();
		}

		if( !pAdo->IsSuccess() ) 
		{ 
			return 0;
		}

		pAdo->NextRecordSet();
		
		if( !pAdo->IsSuccess() ) 
		{ 
			return 0;
		}

		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"fff", wWordValue));
			std::wcout << "fff : " << wWordValue << std::endl;

			pAdo->MoveNext();
		}
		
		if( !pAdo->IsSuccess() ) 
		{ 
			return 0;
		}


		pAdo->NextRecordSet();
		
		if( !pAdo->IsSuccess() )
		{ 
			return 0;
		}

		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"ggg", i64BigIntValue));
			std::wcout << "ggg : " << i64BigIntValue << std::endl;
			
			pAdo->MoveNext();
		}
		
		if( !pAdo->IsSuccess() )
		{ 
			return 0;
		}

		pAdo->NextRecordSet();
		if( !pAdo->IsSuccess() )
		{ 
			return 0;
		}

		while( !pAdo->GetEndOfFile() )
		{
			ISFAIL(pAdo->GetFieldValue(L"hhh", oleTime));
			std::wcout << "hhh : " << oleTime.Format(L"%Y-%m-%d %H:%M:%S") << std::endl;
			
			pAdo->MoveNext();
		}

		if( !pAdo->IsSuccess() )
		{ 
			return 0;
		}

		pAdo->NextRecordSet();

		if( !pAdo->IsSuccess() ) 
		{ 
			return 0;
		}

		while( !pAdo->GetEndOfFile() )
		{
			ISFAIL(pAdo->GetFieldValue(L"iii", oleTime));
			std::wcout << "iii : " << oleTime.Format(L"%Y-%m-%d %H:%M:%S") << std::endl;
			
			pAdo->MoveNext();
		}
		
		if( !pAdo->IsSuccess() )
		{ 
			return 0;
		}

		pAdo->NextRecordSet();
		
		if( !pAdo->IsSuccess() )
		{ 
			return 0;
		}

		while(!pAdo->GetEndOfFile())
		{
			ISFAIL(pAdo->GetFieldValue(L"jjj", fValue));
			std::wcout << "jjj : " << fValue << std::endl;
			
			pAdo->MoveNext();
		}
		if(!pAdo->IsSuccess()){ return 0;}

		pAdo->GetParameter(L"return", nRtnParam);
		pAdo->GetParameter(L"@v_aaa", nParam);
		pAdo->GetParameter(L"@v_bbb", szParam, 100);
		pAdo->GetParameter(L"@v_ccc", pbyDATA, 10, nSize);
		pAdo->GetParameter(L"@v_ddd", bBoolValue);
		pAdo->GetParameter(L"@v_eee", byByteValue);
		pAdo->GetParameter(L"@v_fff", wWordValue);
		pAdo->GetParameter(L"@v_ggg", i64BigIntValue);
		pAdo->GetParameter(L"@v_hhh", oleTime);
		pAdo->GetParameter(L"@v_iii", oleTime);
		pAdo->GetParameter(L"@v_jjj", fValue);
		pAdo->GetParameter(L"@v_kkk", pbyDATA, 10, nSize);
		if(!pAdo->IsSuccess()){return 0;}

		std::wcout << "Return value : " << nRtnParam << std::endl;
		std::wcout << "param out1 : " << nParam << std::endl;
		std::wcout << "param out2 : " << szParam << std::endl;
		std::wcout << "param out4 : " << bBoolValue << std::endl;
		std::wcout << "param out5 : " << byByteValue << std::endl;
		std::wcout << "param out6 : " << wWordValue << std::endl;
		std::wcout << "param out7 : " << i64BigIntValue << std::endl;
		std::wcout << "param out8 : " << oleTime.Format(L"%Y-%m-%d %H:%M:%S") << std::endl;
		std::wcout << "param out9 : " << fValue << std::endl;
		std::wcout << "param out10 : " << fValue;
			
		for(int k = 0; k < nSize; ++k)
		{
			std::wcout << pbyDATA[i];
		}

		std::wcout << std::endl;
		
		--i;
	}


	delete pDBmanager;
	
	getchar();

	return 0;
}