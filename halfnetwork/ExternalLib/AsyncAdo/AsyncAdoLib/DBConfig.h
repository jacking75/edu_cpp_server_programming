#pragma once

#include <string>

namespace asyncadodblib
{
	/// <summary>
	/// DB 설정 정보 </summary>
	class DBConfig
	{
	public:
		DBConfig()
			:m_nConnectionTimeout(0),
			m_nCommandTimeout(0),
			m_bRetryConnection(false), 
			m_bCanWriteErrorLog(false)
		{
		}

		/// <summary>
		/// 접속 정보 설정 </summary>
		void Setting( const std::wstring strDBAddress, 
						const std::wstring strID, 
						const std::wstring strPassWord, 
						const std::wstring strDBName, 
						const int nConnectionTimeOut, 
						const bool bIsRetryConnection, 
						const int nMaxConnectionPool, 
						const bool bCanWriteErrorLog = true
					)
		{
			m_strDataSource = L";Data Source="; 
			m_strDataSource += strDBAddress; 
			SetProvider();

			m_strUserID = strID;
			m_strPassword = strPassWord;
			m_strInitialCatalog = strDBName;

			m_nConnectionTimeout = nConnectionTimeOut;
			m_bRetryConnection = bIsRetryConnection;
			m_nMaxConnectionPool = nMaxConnectionPool;

			m_bCanWriteErrorLog = bCanWriteErrorLog;
		}
				
		/// <summary>
		/// DB DSN 주속 설정 </summary>
		void SetDSN( wchar_t* pszString ) 
		{ 
			m_strDSN = L";DSN="; 
			m_strDSN += pszString;
		}

		/// <summary> ???
		/// 명령 대기 시간 설정. SQL 명령을 내린 후 지정한 시간까지만 처리를 기다림 </summary>
		void SetCommandTimeout( int nCommendTimeout ) { m_nCommandTimeout = nCommendTimeout; }

		std::wstring GetConnectionString()
		{
			if( m_strDataSource.empty() )
			{
				m_strConnectingString = m_strDSN;
			}
			else
			{
				m_strConnectingString = m_strDataSource;
			}

			return m_strConnectingString;
		}

		std::wstring& GetUserID()		{return m_strUserID; }
		
		std::wstring& GetPassword()		{return m_strPassword; }
		
		std::wstring& GetInitCatalog()	{return m_strInitialCatalog; }
		
		std::wstring& GetProvider()		{return m_strProvider; }
		
		int GetConnectionTimeout()		{ return m_nConnectionTimeout; }
		
		bool IsCanRetryConnection()		{ return m_bRetryConnection; }
		
		int GetMaxConnectionPool()		{ return m_nMaxConnectionPool; }

		bool CanWriteErrorLog()		{ return m_bCanWriteErrorLog; }


	private:
		/// <summary> DB API 프로바이더 설정 </summary>
		void SetProvider( wchar_t* pString = L"SQLOLEDB.1" )
		{
			m_strProvider = pString;
		}


		/// <summary> 연결 문자열 </summary>
		std::wstring m_strConnectingString;
		
		/// <summary> 사용할 데이터베이스 이름 </summary>
		std::wstring m_strInitialCatalog;
		
		/// <summary> 데이터베이스 주속 </summary>
		std::wstring m_strDataSource;

		/// <summary> DB 접속 아이디 </summary>
		std::wstring m_strUserID;

		/// <summary> DB 접속 패스워드 </summary>
		std::wstring m_strPassword;

		/// <summary> DB API 프로바이더 </summary>
		std::wstring m_strProvider;

		/// <summary> DB의 DSN 주소 </summary>
		std::wstring m_strDSN;
		
		/// <summary>  </summary>
		int m_nConnectionTimeout;

		/// <summary> ?? </summary>
		int m_nCommandTimeout;

		/// <summary> 재연결 여부 </summary>
		bool m_bRetryConnection;

		/// <summary> 연결 풀의 최대 개수 </summary>
		int m_nMaxConnectionPool;
					
		/// <summary> 에러 로그 출력 가능 여부 </summary>
		bool m_bCanWriteErrorLog;
	};
}