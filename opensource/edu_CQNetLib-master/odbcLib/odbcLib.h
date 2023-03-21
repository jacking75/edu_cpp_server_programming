#pragma once

#include <iostream>

#include <windows.h>
#include <sql.h>
#include <sqlext.h>

#define ERRORSQL_RETSIZE	-101
#define ERRORSQL_NORMAL		-100
#define MAX_ERRORMSG		1024 * 4



namespace MyODBCLib
{
	/*
	DBWorker 클래스:ODBC의 래퍼 클래스. 
	- API 수준에서 재사용 가능하도록 작성하였다.
	- SQL문을 편리하게 실행하고 결과를 쉽게 읽는데 초점을 맞추었으며 SQL 서버와 액세스에서 완벽하게 테스트 되었다.
	- 255자 이하의 정수, 문자열 컬럼에 대해서만 사용 가능하며 그 이상의 길이를 가지는 필드는 "오른쪽 잘림" 진단 경고가 발생하므로 이 클래스로 읽을 수 없으며 ODBC API 함수를 직접 사용해야 한다.
	- 최대 컬럼 개수는 100개이다. 초과시 사정없이 에러 메시지 박스를 출력하므로 이 한도를 알아서 넘지 않도록 주의해야 한다. 특히 필드수가 많은 테이블을 select * 로 읽는 것은 삼가하는 것이 좋다.
	- 전진 전용 커서를 사용하므로 이미 읽은 레코드는 쿼리를 다시 실행해야만 읽을 수 있다.

	사용법
	1.DBWorker 클래스의 객체를 생성한다. 가급적이면 전역으로 선언하는 것이 좋으며 필요한 수만큼 객체를 만든다. 보통 세 개 정도면 충분하다.
	2.적절한 Connect 함수를 호출하여 데이터 소스에 연결한다.
	3.Exec 함수로 SQL문을 실행한다. 에러 처리는 Exec 함수 내부에서 처리한다.
	4.Fetch 함수로 결과 셋 하나를 가져온다. 여러개의 결과셋이 있는 경우는 while 루프를 돌면서 차례대로 꺼내올 수 있다.
	5.Get* 함수로 컬럼 값을 읽는다.
	6.Clear 함수로 명령 핸들을 닫는다.(Select문일 경우만)
	7.객체를 파괴한다. 전역 객체인 경우는 일부러 파괴할 필요가 없다.
	*/
	class DBWorker
	{
		// 최대 컬럼수, BLOB 입출력 단위, NULL 필드값
		enum {
			MAXCOL = 100, BLOBBATCH = 10000, cQueryNULL = -100, cQueryEOF = -101,
			cQueryNOCOL = -102, cQueryERROR = -103
		};

	public:
		DBWorker()
		{
			std::cout << "SYSTEM | cQuery::cQuery() | cQuery 생성자 호출" << std::endl;
			AffectCount = -1;
			ret = SQL_SUCCESS;
			hStmt = NULL;
			hDbc = NULL;
			hEnv = NULL;
			m_bIsConnect = false;
			ZeroMemory(m_szErrorMsg, MAX_ERRORMSG);
		}

		virtual ~DBWorker()
		{
			std::cout << "SYSTEM | cQuery::~cQuery() | cQuery 소멸자 호출" << std::endl;
			DisConnect();
		}
		
		// 에러 발생시 진단 정보를 문자열로 만들어 준다
		void PrintDiag()
		{
			INT32 ii;
			SQLRETURN Ret;
			SQLINTEGER NativeError;
			SQLCHAR SqlState[6], Msg[255];
			SQLSMALLINT MsgLen;

			ii = 1;
			/*while (Ret=SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ii, SqlState, &NativeError,
				Msg, sizeof(Msg), &MsgLen)!=SQL_NO_DATA) {*/
			bool bLoop = true;
			while (bLoop)
			{
				Ret = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ii, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen);
				if (SQL_NO_DATA == Ret) {
					break;
				}

				//연결이 끊겼다면
				if (_strcmpi((LPCTSTR)SqlState, "08S01") == 0)
					m_bIsConnect = false;

				sprintf(m_szErrorMsg, "SQLSTATE(%s) 진단정보(%s)", (LPCTSTR)SqlState, (LPCTSTR)Msg);
				printf("[DBWorker::PrintDiag] %s", m_szErrorMsg);

				++ii;
			}
		}

		inline char* GetErrorMsg() { return m_szErrorMsg; }
		inline bool GetIsConnect() { return m_bIsConnect; }
		
		void DisConnect()
		{
			if (hStmt) {
				SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
			}

			if (hDbc) {
				SQLDisconnect(hDbc);
			}

			if (hDbc) {
				SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
			}

			if (hEnv) {
				SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
			}

			m_bIsConnect = false;
		}

		// 데이터 소스에 연결한다
		// 연결 핸들을 할당하고 연결한 후 명령핸들까지 같이 할당한다.
		// Type=1:ConStr은 MDB 파일의 경로를 가진다. 경로 생략시 현재 디렉토리에서 MDB를 찾는다.
		// Type=2:ConStr은 SQL 서버의 연결 정보를 가지는 DSN 파일의 경로를 가진다. 
		//        경로는 반드시 완전 경로로 지정해야 한다.
		// Type=3:SQLConnect 함수로 DSN에 직접 연결한다.
		// 연결 또는 명령 핸들 할당에 실패하면 FALSE를 리턴한다.
		BOOL Connect(INT32 Type, const char* ConStr, const char* UID = NULL, const char* PWD = NULL, bool bIsPrint = true)
		{
			ZeroMemory(m_szErrorMsg, MAX_ERRORMSG);

			//이미 연결이 되어있다면 연결을 끊는다.
			if (m_bIsConnect)
				DisConnect();

			SQLCHAR InCon[255];
			SQLCHAR OutCon[255];
			SQLSMALLINT cbOutCon;
			m_bIsPrint = bIsPrint;
			INT32 ii = 1;
			SQLRETURN Ret;
			SQLINTEGER NativeError;
			SQLCHAR SqlState[6], Msg[255];
			SQLSMALLINT MsgLen;

			// 환경 핸들을 할당하고 버전 속성을 설정한다.
			SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
			SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
			// 연결 타입에 따라 MDB 또는 SQL 서버, 또는 DSN에 연결한다.
			SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
			switch (Type) {
			case 1:
				wsprintf((char*)InCon, "DRIVER={Microsoft Access Driver (*.mdb)};DBQ=%s;", ConStr);
				ret = SQLDriverConnect(hDbc, NULL, (SQLCHAR*)InCon, sizeof(InCon), OutCon,
					sizeof(OutCon), &cbOutCon, SQL_DRIVER_NOPROMPT);
				break;
			case 2:
				wsprintf((char*)InCon, "FileDsn=%s", ConStr);
				ret = SQLDriverConnect(hDbc, NULL, (SQLCHAR*)InCon, sizeof(InCon), OutCon,
					sizeof(OutCon), &cbOutCon, SQL_DRIVER_NOPROMPT);
				break;
			case 3:
				ret = SQLConnect(hDbc, (SQLCHAR*)ConStr, SQL_NTS, (SQLCHAR*)UID, SQL_NTS,
					(SQLCHAR*)PWD, SQL_NTS);
				break;
			}

			// 접속 에러시 진단 정보를 보여준다.
			if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
			{
				/*while (Ret=SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, ii, SqlState, &NativeError,
					Msg, sizeof(Msg), &MsgLen)!=SQL_NO_DATA)*/
				bool bLoop = true;
				while (bLoop)
				{
					Ret = SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, ii, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen);
					if (SQL_NO_DATA == Ret) {
						break;
					}

					printf("SYSTEM | cQuery::Connect() | SQLSTATE(%s), 진단정보(%s)", (LPCTSTR)SqlState, (LPCTSTR)Msg);

					++ii;
				}
				return FALSE;
			}

			// 명령 핸들을 할당한다.
			ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
			if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) 
			{
				hStmt = 0;
				return FALSE;
			}

			//Exec("f");
			m_bIsConnect = true;
			return TRUE;
		}

		// SQL문을 실행한다. 실패시 진단 정보를 출력하고 FALSE를 리턴한다.
		BOOL Exec(LPCTSTR szSQL)
		{
			INT32 c;

			// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
			// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
			ret = SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);
			if ((ret != SQL_SUCCESS) && (ret != SQL_NO_DATA)) {
				PrintDiag();
				return FALSE;
			}

			// Update, Delete, Insert 명령시 영향받은 레코드 개수를 구해 놓는다.
			SQLRowCount(hStmt, &AffectCount);
			SQLNumResultCols(hStmt, &nCol);
			if (nCol > MAXCOL) {
				printf("SYSTEM | cQuery::Exec() | 최대 컬럼 수를 초과했습니다");
				return FALSE;
			}

			// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
			// 바인딩을 할 필요가 없다.
			if (nCol == 0) {
				Clear();
				return TRUE;
			}

			// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
			// 컬럼 번호는 one base임에 유의할 것
			for (c = 0; c < nCol; c++) {
				SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 255, &lCol[c]);

			}
			return TRUE;
		}

		// SQL문을 실행하고 결과셋의 첫 칼럼에서 정수값을 읽어 리턴해 준다. 
		// 결과를 얻은 후 정리까지 해 준다.
		INT32 ExecGetInt(LPCTSTR szSQL)
		{
			INT32 i;

			if (Exec(szSQL) == FALSE)
				return cQueryERROR;

			// 데이터가 없는 경우 EOF를 리턴한다.
			if (Fetch() == SQL_NO_DATA) {
				Clear();
				return cQueryEOF;
			}
			i = GetInt(1);
			Clear();
			return i;
		}

		// SQL문을 실행하고 결과셋의 첫 칼럼에서 문자열을 읽어 리턴해 준다.
		INT32 ExecGetStr(LPCTSTR szSQL, char* buf)
		{
			// SQL문 실행중 에러가 발생한 경우 문자열 버퍼에 에러를 돌려 보낸다.
			if (Exec(szSQL) == FALSE) {
				return -1;
			}

			// 데이터가 없는 경우 EOF를 리턴한다.
			if (Fetch() == SQL_NO_DATA) {
				Clear();
				return SQL_NO_DATA;
			}
			GetStr(1, buf);
			Clear();

			return 0;
		}

		// SQL문을 실행하고 스토어드 프로시져에서 리턴값을 돌려줌.
		INT32 ExecGetReturn(LPCTSTR szSQL)
		{
			INT32 c;

			INT32 nRetProc = 0;	//스토어드 프로시져에서 받은 반환값
			SQLLEN nRetSize = 0;//반환값의 크기

			ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT_OUTPUT, SQL_INTEGER, SQL_INTEGER, 0, 0,
				&nRetProc, 0, (SQLLEN*)& nRetSize);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
			// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
			ret = SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);

			//여기서 리턴하지 않는 이유는 에러코드를 확인하기 위해서 그렇다.
			//그런데 또 밑에서 한꺼번에 하지 않는 이유는 PrintDiag()전에 OUTPUTSTRING을 하면
			//에러 메시지가 다 날라가서 그렇다.. 
			//이 구조는 구조적인 문제로써 cQuery를 전체적으로 바꿔야하겠다.
			if ((ret != SQL_SUCCESS) && (ret != SQL_NO_DATA)) {
				PrintDiag();

			}
#ifdef ERROROUT
			OUTPUTSTRING;
#endif
			if ((0 != nRetProc) || (0 == nRetSize))
			{
				PrintDiag();
				//함수 자체가 실패해서 리턴값을 받지 못했다면
				if (0 == nRetSize)
					return ERRORSQL_RETSIZE;
				return nRetProc;
			}
			if ((ret != SQL_SUCCESS) && (ret != SQL_NO_DATA)) {
				return ret;
			}


			// Update, Delete, Insert 명령시 영향받은 레코드 개수를 구해 놓는다.
			SQLRowCount(hStmt, &AffectCount);
			SQLNumResultCols(hStmt, &nCol);
			if (nCol > MAXCOL) 
			{
				printf("cQuery::ExecGetReturn() | 최대 컬럼 수를 초과했습니다");
				return ERRORSQL_NORMAL;
			}

			// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
			// 바인딩을 할 필요가 없다.
			if (nCol == 0) {
				Clear();
				return 0;
			}

			// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
			// 컬럼 번호는 one base임에 유의할 것
			for (c = 0; c < nCol; c++) {
				SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 255, &lCol[c]);

			}
			return 0;
		}

		// SQL문을 실행하고 스토어드 프로시져에서 리턴값과 한개의 출력값을 돌려줌.
		INT32 ExecGetReturnAndOutput(LPCTSTR szSQL, __int64& n64Output)
		{
			INT32 c;
			//스토어드 프로시져에서 받은 반환값
			INT32 nRetProc1 = 0;
			__int64 n64RetProc2 = 0;
			//반환값의 크기
			SQLLEN nRetSize1 = 0, nRetSize2 = 0;

			ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
				&nRetProc1, 0, (SQLLEN*)& nRetSize1);
			ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0,
				&n64RetProc2, 0, (SQLLEN*)& nRetSize2);

			if ((ret != SQL_SUCCESS))
			{
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
			// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
			ret = SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);

			if ((ret != SQL_SUCCESS) && (ret != SQL_NO_DATA)) {
				PrintDiag();
			}
#ifdef ERROROUT
			OUTPUTSTRING;
#endif
			//반환값과 반환값의 크기를 판단하여 오류를 검출
			if ((0 != nRetProc1) || (0 == nRetSize1) || (0 == nRetSize2))
			{
				PrintDiag();
				//함수 자체가 실패해서 리턴값을 받지 못했다면
				if (0 == nRetSize1)
					return ERRORSQL_RETSIZE;
				return nRetProc1;
			}

			n64Output = n64RetProc2;
			// Update, Delete, Insert 명령시 영향받은 레코드 개수를 구해 놓는다.
			SQLRowCount(hStmt, &AffectCount);
			SQLNumResultCols(hStmt, &nCol);
			if (nCol > MAXCOL) 
			{
				printf("cQuery::ExecGetReturn() | 최대 컬럼 수를 초과했습니다");
				return ERRORSQL_NORMAL;
			}

			// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
			// 바인딩을 할 필요가 없다.
			if (nCol == 0) {
				Clear();
				return 0;
			}

			// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
			// 컬럼 번호는 one base임에 유의할 것
			for (c = 0; c < nCol; c++) {
				SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 255, &lCol[c]);

			}
			return 0;
		}
		
		INT32 ExecGetReturnAndOutput3(LPCTSTR szSQL, __int64& n64Output1, __int64& n64Output2, __int64& n64Output3)
		{
			INT32 c;
			INT32 nRetProc1 = 0;
			__int64 n64OutPut1 = 0, n64OutPut2 = 0, n64OutPut3 = 0;
			SQLLEN nRetSize1 = 0, nRetSize2 = 0, nRetSize3 = 0, nRetSize4 = 0;


			ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nRetProc1, 0, (SQLLEN*)& nRetSize1);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}

			ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0, &n64OutPut1, 0, (SQLLEN*)& nRetSize2);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}

			ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0, &n64OutPut2, 0, (SQLLEN*)& nRetSize3);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}

			ret = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0, &n64OutPut3, 0, (SQLLEN*)& nRetSize4);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}


			// Update, Delete, Insert 명령시 영향받은 레코드 개수를 구해 놓는다.
			SQLRowCount(hStmt, &AffectCount);
			SQLNumResultCols(hStmt, &nCol);
			if (nCol > MAXCOL) 
			{
				printf("cQuery::ExecGetReturn() | 최대 컬럼 수를 초과했습니다");
				return ERRORSQL_NORMAL;
			}
			INT32 nColResult = nCol;

			// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
			// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
			ret = SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);

			if ((ret != SQL_SUCCESS) && (ret != SQL_NO_DATA)) {
				PrintDiag();
			}

#ifdef ERROROUT
			OUTPUTSTRING;
#endif

			//반환값과 반환값의 크기를 판단하여 오류를 검출
			if ((0 != nRetProc1) || (0 == nRetSize1) || (0 == nRetSize2) || (0 == nRetSize3) || (0 == nRetSize4))
			{
				PrintDiag();

				//함수 자체가 실패해서 리턴값을 받지 못했다면
				if (0 == nRetSize1)
					return ERRORSQL_RETSIZE;
				return nRetProc1;
			}

			n64Output1 = n64OutPut1;
			n64Output2 = n64OutPut2;
			n64Output3 = n64OutPut3;


			// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
			// 바인딩을 할 필요가 없다.
			if (nColResult == 0) {
				Clear();
				return 0;
			}

			// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
			// 컬럼 번호는 one base임에 유의할 것
			for (c = 0; c < nColResult; c++) {
				SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 255, &lCol[c]);

			}

			return 0;
		}
		
		INT32 ExecGetReturnAndOutput4(LPCTSTR szSQL, INT32& nOutput1, __int64& n64Output2, __int64& n64Output3, INT32& nOutput4)
		{
			INT32 c;
			INT32 nRetProc1 = 0, nRetProc2 = 0, nRetProc5 = 0;	//스토어드 프로시져에서 받은 반환값
			__int64 n64RetProc3 = 0, n64RetProc4 = 0;
			SQLLEN nRetSize1 = 0, nRetSize2 = 0, nRetSize3 = 0, nRetSize4 = 0, nRetSize5 = 0;//반환값의 크기

			ret = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
				&nRetProc1, 0, (SQLLEN*)& nRetSize1);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			ret = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
				&nRetProc2, 0, (SQLLEN*)& nRetSize2);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			ret = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0,
				&n64RetProc3, 0, (SQLLEN*)& nRetSize3);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			ret = SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT_OUTPUT, SQL_C_SBIGINT, SQL_INTEGER, 0, 0,
				&n64RetProc4, 0, (SQLLEN*)& nRetSize4);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			ret = SQLBindParameter(hStmt, 5, SQL_PARAM_INPUT_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0,
				&nRetProc5, 0, (SQLLEN*)& nRetSize5);
			if ((ret != SQL_SUCCESS)) {
				PrintDiag();
				return ERRORSQL_NORMAL;
			}
			// Update, Delete, Insert 명령시 영향받은 레코드 개수를 구해 놓는다.
			SQLRowCount(hStmt, &AffectCount);
			SQLNumResultCols(hStmt, &nCol);
			if (nCol > MAXCOL) 
			{
				printf("cQuery::ExecGetReturn() | 최대 컬럼 수를 초과했습니다");
				return ERRORSQL_NORMAL;
			}
			INT32 nColResult = nCol;

			// SQL문을 실행한다. SQL_NO_DATA를 리턴한 경우도 일단 성공으로 취급한다. 
			// 이 경우 Fetch에서 EOF를 리턴하도록 되어 있기 때문에 진단 정보를 출력할 필요는 없다.
			ret = SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);

			if ((ret != SQL_SUCCESS) && (ret != SQL_NO_DATA)) {
				PrintDiag();
			}
#ifdef ERROROUT
			OUTPUTSTRING;
#endif
			//반환값과 반환값의 크기를 판단하여 오류를 검출
			if ((0 != nRetProc1) || (0 == nRetSize1) || (0 == nRetSize2) || (0 == nRetSize3) || (0 == nRetSize4) || (0 == nRetSize5))
			{
				PrintDiag();
				//함수 자체가 실패해서 리턴값을 받지 못했다면
				if (0 == nRetSize1)
					return ERRORSQL_RETSIZE;
				return nRetProc1;
			}

			nOutput1 = nRetProc2;
			n64Output2 = n64RetProc3;
			n64Output3 = n64RetProc4;
			nOutput4 = nRetProc5;


			// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
			// 바인딩을 할 필요가 없다.
			if (nColResult == 0) {
				Clear();
				return 0;
			}

			// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
			// 컬럼 번호는 one base임에 유의할 것
			for (c = 0; c < nColResult; c++) {
				SQLBindCol(hStmt, c + 1, SQL_C_CHAR, Col[c], 255, &lCol[c]);

			}
			return 0;
		}
		
		// 결과셋에서 한 행을 가져온다.
		SQLRETURN Fetch()
		{
			ret = SQLFetch(hStmt);
			return ret;
		}
		
		// 커서를 닫고 바인딩 정보를 해제한다.
		void Clear()
		{
#ifdef ERROROUT
			OUTPUTSTRING;
#endif
			SQLCloseCursor(hStmt);
			SQLFreeStmt(hStmt, SQL_UNBIND);
		}
		
		// nCol의 컬럼값을 정수로 읽어준다. NULL일 경우 cQueryNULL을 리턴한다.
		INT32 GetInt(INT32 nCol)
		{
			if (nCol > this->nCol)
				return cQueryNOCOL;

			if (lCol[nCol - 1] == SQL_NULL_DATA) {
				return cQueryNULL;
			}
			else {
				return atoi(Col[nCol - 1]);

			}
		}
		
		// __int64 정수형 컬럼 읽기
		unsigned __int64 GetInt64(INT32 nCol)
		{
			if (nCol > this->nCol)
				return static_cast<unsigned __int64>(cQueryNOCOL);

			if (blCol[nCol - 1] == SQL_NULL_DATA) {
				return static_cast<unsigned __int64>(cQueryNULL);
			}
			else {
				unsigned __int64 ret = _atoi64(Col[nCol - 1]);
				return ret;
			}
		}
		
		// 문자열형 컬럼 읽기
		// nCol의 컬럼값을 문자열로 읽어준다. NULL일 경우 문자열에 NULL을 채워준다. 
		// buf의 길이는 최소한 256이어야 하며 길이 점검은 하지 않는다.
		bool GetStr(INT32 nCol, char* buf)
		{
			if (nCol > this->nCol) {
				return false;
			}

			if (lCol[nCol - 1] == SQL_NULL_DATA) {
				return false;
			}
			else {
				lstrcpy(buf, Col[nCol - 1]);
			}

			return true;
		}
		
		char GetChar(INT32 nCol)
		{
			if (nCol > this->nCol) {
				return 0x00;
			}

			if (lCol[nCol - 1] == SQL_NULL_DATA) {
				return 0x00;
			}

			return Col[nCol - 1][0];
		}
		
		// BLOB 데이터를 buf에 채워준다. 이때 buf는 충분한 크기의 메모리를 미리 할당해 놓아야 한다. NULL일 경우 0을 리턴하고 에러 발생시 -1을 리턴한다. 
		// 성공시 읽은 총 바이트 수를 리턴한다. szSQL은 하나의 BLOB 필드를 읽는 Select SQL문이어야 한다.
		INT32 ReadBlob(LPCTSTR szSQL, void* buf)
		{
			SQLCHAR BinaryPtr[BLOBBATCH];
			SQLLEN LenBin;
			char* p;
			INT32 nGet;
			INT32 TotalGet = 0;

			ret = SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);
			if (ret != SQL_SUCCESS) {
				PrintDiag();
				return -1;
			}

			while ((ret = SQLFetch(hStmt)) != SQL_NO_DATA) {
				p = (char*)buf;
				while ((ret = SQLGetData(hStmt, 1, SQL_C_BINARY, BinaryPtr, sizeof(BinaryPtr),
					&LenBin)) != SQL_NO_DATA) {
					if (LenBin == SQL_NULL_DATA) {
						Clear();
						return 0;
					}
					if (ret == SQL_SUCCESS)
						nGet = LenBin;
					else
						nGet = BLOBBATCH;
					TotalGet += nGet;
					memcpy(p, BinaryPtr, nGet);
					p += nGet;
				}
			}
			Clear();
			return TotalGet;
		}
		
		// buf의 BLOB 데이터를 저장한다. szSQL은 하나의 BLOB 데이터를 저장하는 Update, Insert SQL문이어야 한다.
		void WriteBlob(LPCTSTR szSQL, void* buf, INT32 size)
		{
			SQLLEN cbBlob;
			char tmp[BLOBBATCH], * p;
			SQLPOINTER pToken;
			INT32 nPut;

			cbBlob = SQL_LEN_DATA_AT_EXEC(size);
			SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY,
				size, 0, (SQLPOINTER)1, 0, &cbBlob);
			SQLExecDirect(hStmt, (SQLCHAR*)szSQL, SQL_NTS);
			ret = SQLParamData(hStmt, &pToken);
			while (ret == SQL_NEED_DATA) {
				if (ret == SQL_NEED_DATA) {
					if (pToken == (SQLPOINTER)1) {
						for (p = (char*)buf; p < (char*)buf + size; p += BLOBBATCH) {
							nPut = min(BLOBBATCH, (char*)buf + size - p);
							memcpy(tmp, p, nPut);
							SQLPutData(hStmt, (PTR)tmp, nPut);
						}
					}
				}
				ret = SQLParamData(hStmt, &pToken);
			}
			Clear();
		}

		double GetFloat(INT32 nCol)
		{
			if (nCol > this->nCol)
				return cQueryNOCOL;

			if (lCol[nCol - 1] == SQL_NULL_DATA) {
				return cQueryNULL;
			}
			else {
				return atof(Col[nCol - 1]);

			}
		}


		

		
		SQLLEN AffectCount;					// 영향받은 레코드 개수
		SQLHSTMT hStmt;							// 명령 핸들. 직접 사용할 수도 있으므로 public으로 정의
		SQLSMALLINT nCol;						// 컬럼 개수
		SQLCHAR ColName[MAXCOL][50];			// 컬럼의 이름들
		SQLLEN lCol[MAXCOL];				// 컬럼의 길이/상태 정보
		SQLUBIGINT blCol[MAXCOL];				// 컬럼의 길이/상태 정보

	protected:
		SQLHENV hEnv;							// 환경 핸들
		SQLHDBC hDbc;							// 연결 핸들
		SQLRETURN ret;							// 최후 실행한 SQL문의 결과값
		char Col[MAXCOL][255];					// 바인딩될 컬럼 정보
		bool m_bIsPrint;						//	디버깅 메세지를 찍을거냐?
		bool m_bIsConnect;						//	연결이 되어 있는지
		char m_szErrorMsg[MAX_ERRORMSG];				// 에러 스트링이 들어있다.		
	};



	class QueryCleaner
	{
	public:

		explicit QueryCleaner(DBWorker& worker) : m_pDBWorker(&worker) {}

		explicit QueryCleaner(DBWorker* worker) : m_pDBWorker(worker) {}

		~QueryCleaner() { m_pDBWorker->Clear(); }

	private:
		DBWorker* m_pDBWorker;
	};


	// ======================================================================
	// 마지막 결과 집합을 처리하는 동안, SQL_NO_DATA를 반환할 때까지 결과 집합을 처리한다.
	// ======================================================================
	// 데이터를 sp에서 리턴 받을때 이것을 이용해서 리턴받으면 된다.
	// 안그러면 리턴받은값이 잘못되있을수 있다.
	//while ( (sRetCode=SQLMoreResults(hStmt)) != SQL_NO_DATA ) ;

#ifdef ERRORSQL
#define OUTPUTSTRING while( SQLMoreResults(hStmt) != SQL_NO_DATA );
#else
#define OUTPUTSTRING 
#endif

}