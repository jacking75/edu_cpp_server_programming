# CQNetLib
C++ IOCP 네트워크 라이브러리  
이 라이브러리를 분석할 때 도서 [온라인 게임 서버`(강정중)](http://www.yes24.com/Product/Goods/1776627 )을 보는 것을 추천한다. 다만 이 책은 이미 절판이라서 찾기 힘들 수도 있다.....  
  
`coding_rule.md` 문서에 있는 코딩 룰을 따라한다 
    
    
## 사용 방법
- lib으로 사용하기
    - 애플리케이션 프로젝트를 만든 후 CQNetLib 프로젝트를 추가 후 참조에 추가한다
- head only로 사용하기
    - `IocpServer.h'를 추가하기 전에 '#define CQNET_USE_HEAD_ONLY 1'을 선언한다

  		
<br/>  
    
## TODO
### 라이브러리
- [ ] loguru를 로그로 사용하도록 한다
- [ ] modern c++
- [V] 덩키 큰 함수, 덩치 큰 파일 쪼개기
- [ ] public 안 해도 되는 멤버는 public으로 안하도록 한다
- [ ] 코드에 있는 TODO 처리하기
- [V] 패킷 분석은 외부에서 변경할 수 있도록.
- [ ] 설정 정보 하드 코딩에서 config로 변경
- [ ] 사용하지 않는 코드 삭제하기
  
### 예제 프로그램
- [ ] 설정 정보는 flag 사용하기
	 
<br/>  
  
  
## 예제 코드 
  
### Echo Server
  
  
### Simple Chat Server 
  
<br/>  
    
	
## Temp
  
### VBuffer
  
```
m_pVBuffer = new cVBufferImpl(1024*16);

bool cGameServer::funcLoadSubjectiveQuiz()
{
	cMonitor::Owner lock( this );

	{
		cQuery::Owner Clear( m_pDBConn );
		
		_snprintf( m_szQueryShortStr, MAX_SHORTQUERY, "EXEC SP_LOAD_SUBJECTIVE_QUIZ" );
		if( m_pDBConn->Exec( m_szQueryShortStr ) == false )
		{
			LOG( LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcLoadSubjectiveQuiz() | EXEC SP_LOAD_SUBJECTIVE_QUIZ 쿼리 실패" );
			return false;
		}

		char* pMarkQuizCount = NULL;
		int iCount = 0;
		while( true )
		{
			char	acQuestion[MAX_SUBJECTIVE_QUESTION_LEN] = {0,};

			// 버퍼 초기화를 한다.
			if( 0 == iCount )
			{
				m_pVBuffer->Init();
				m_pVBuffer->SetShort( DTG_LOAD_SUBJECTIVEQUIZ_AQ );
				pMarkQuizCount = m_pVBuffer->GetCurMark();
				m_pVBuffer->SetInteger( iCount );
			}

			SQLRETURN retcode = m_pDBConn->Fetch();
			if( SQL_NO_DATA == retcode || -1 == retcode ) {
				break;
			}
			
			// DB 데이터를 인코딩 한다.
			m_pVBuffer->SetInteger( m_pDBConn->GetInt( SUBJECTIVEQUIZ_CD ) );
			m_pVBuffer->SetChar( m_pDBConn->GetInt( SUBJECTIVEQUIZ_KIND ) );
			m_pDBConn->GetStr( SUBJECTIVEQUIZ_QUESTION, acQuestion );	m_pVBuffer->SetString( acQuestion );
			
			++iCount;

			// 지정된 개수가 되면 보낸다.
			if( 100 == iCount )
			{
				CopyMemory( pMarkQuizCount, &iCount, sizeof( iCount ) );
				SendMemberVBuffer();

				iCount = 0;
			}
		}
		
		// 위에서 아직 보내지 못한 것이 있다면 보낸다.
		if( iCount > 0 )
		{
			CopyMemory( pMarkQuizCount, &iCount, sizeof( iCount ) );
			SendMemberVBuffer();
		}
	}
	

	// 문제의 답변을 보낸다.
	funcLoadSubjectiveQuizAnswer();
	
	return true;
}

bool cGameServer::SendMemberVBuffer()
{
	return SendBuffer( m_pVBuffer->GetBeginMark() , m_pVBuffer->GetCurBufSize() );
}

```  
  
### 서비스 모드로 사용할 때   
  
```
/*----------------------------------------------------------------------------------------------------
// 윈도우 서비스 모드로 실행될 경우
*---------------------------------------------------------------------------------------------------*/
if( nPos > 0 )
{
	g_pWindowsService->CreateLog( "c:\\strokerserver_bin\\[SERVICE]LoginServer.log" );
	szServiceName = szServiceName.Mid( nPos + 1, szServiceName.GetLength() - (nPos + 1) );
	g_pWindowsService->OutputLog( LOG_INFO_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | =====================================================================================================" );
	g_pWindowsService->OutputLog( LOG_INFO_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | 서비스 초기화.." );
	g_pWindowsService->OutputLog( LOG_INFO_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | 메니지 서버 접속 시도" );
	
	//매니저 서버 접속
	//if( !g_pManageServerComm->ConnectToManageServer() )
	//{
	//	g_pWindowsService->OutputLog( LOG_ERROR_NORMAL , "SYSTEM | CLoginServerApp::InitInstance() | 메니지 서버 실패" );
	//	return false;
	//}
	
	g_pWindowsService->OutputLog( LOG_INFO_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | 메니지 서버 접속 성공" );
	g_pWindowsService->InitService( szServiceName.GetBuffer() );

	g_pWindowsService->OutputLog( LOG_INFO_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | 서비스 시작.." );

	//서비스 프로세스를 시작한다. (서비스 종료 전까지는 리턴되지 않는다.)
	if( szServiceName == "DEBUG" )
		g_pWindowsService->ServiceStart( 100 , 0 );
	else
	{
		if( g_pWindowsService->ServiceStart() == false )
			g_pWindowsService->OutputLog( LOG_ERROR_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | 서비스 시작 실패..." );
	}
	g_pWindowsService->OutputLog( LOG_INFO_NORMAL ,"SYSTEM | CLoginServerApp::InitInstance() | 서비스 끝냄.." );
	
	//서비스 프로세스를 종료한다.
	//g_pGameDBIocpServer->ServerOff();

	return FALSE;
}
```
  