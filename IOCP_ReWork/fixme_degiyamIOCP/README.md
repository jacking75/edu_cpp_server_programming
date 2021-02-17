### 코드를 수정해야 하는 IOCP 네트워크 라이브러리.
'온라인 서버 제작자 모임'에 degiyam 이라는 닉네임의 개발자분이 공개한것.
<br>
이 프로젝트를 클론한 후 수정해 보기 바란다.(수정 후 알려주면 여기에 링크를 게시하겠다^^)

### 개선할 부분
- 멀티바이트 문자열을 모두 유니코드 문자열 사용으로
    - _tcslen 함수를 사용하지 않도록 한다
- LIB_LOGSYSTEM::WriteLogHandle의 WriteFile 사용 시 lock 불 필요
- LIB_SESSIONPOOL 클래스에 미 구현 함수 구현하기
- 패킷 보내기 에러 ServiceManager::SendThread()
    - 패킷 보내기 완료까지 데이터 관리 하지 않음
    - SetEvent 잘못된 사용
	```
	const BOOL ServiceManager::RecvCS_AUTH_LOGIN_ACK(LIB_SESSIONDATA* pSession)
	{
		ZeroMemory(pSession->m_SocketCtx.sendContext, MAX_BUFFER);

		SYSTEMTIME st;
		GetSystemTime(&st);

		TCHAR	szTime[MAX_STRING] = _T("");

		wsprintf(szTime, "%d/%d/%ds/%dms", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

		memcpy(&pSession->m_SocketCtx.sendContext->Buffer, szTime, strlen(szTime));

		m_Lock.Lock();
		{
			m_SendCtx = pSession;
			SetEvent(m_hSendEvent);
		}
		m_Lock.UnLock();

		return 1;
	}
	```

- 패킷 뭉침 처리 안함, 공유 객체 문제가 발생할 수 있음
    - ServiceManager::WorkerThread()
    - 받은 데이터를 하나의 패킷으로 보고 바로 처리
    - IOCP 스레드에서 바로 패킷 처리


### Send 개선 아이디어
- queue 이용

```
std::deque< char* > m_SendDataQueue;

void Session::PostSend( const bool bImmediately, const int nSize, char* pData )
{
	char* pSendData = nullptr;

	if( bImmediately == false )
	{
		pSendData = new char[nSize];
		memcpy( pSendData, pData, nSize);

		m_SendDataQueue.push_back( pSendData );
	}
	else
	{
		pSendData = pData;
	}



	if (bImmediately == false && m_SendDataQueue.size() > 1)
	{
		return;
	}

	boost::asio::async_write( m_Socket, boost::asio::buffer( pSendData, nSize ),
							 boost::bind( &Session::handle_write, this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred )
							);
}

void Session::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
	delete[] m_SendDataQueue.front();
	m_SendDataQueue.pop_front();

	if( m_SendDataQueue.empty() == false )
	{
		char* pData = m_SendDataQueue.front();

		PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;

		PostSend( true, pHeader->nSize, pData );
	}
}
```
- 버퍼 이용

```
bool Session::PostSend(const char* data, size_t len)
{
	TRACE_THIS;

	if (!IsConnected())
		return false;

	FastSpinlockGuard criticalSection(mSendBufferLock);

	if (mSendBuffer.GetFreeSpaceSize() < len)
		return false;

	/// flush later...
	LSendRequestSessionList->push_back(this);

	char* destData = mSendBuffer.GetBuffer();

	memcpy(destData, data, len);

	mSendBuffer.Commit(len);

	return true;
}

void IOThread::DoSendJob()
{
	while (!LSendRequestSessionList->empty())
	{
		auto& session = LSendRequestSessionList->front();

		if (session->FlushSend())
		{
			/// true 리턴 되면 빼버린다.
			LSendRequestSessionList->pop_front();
		}
	}

}

bool Session::FlushSend()

case IO_SEND:
	remote->SendCompletion(dwTransferred);

	if (context->mWsaBuf.len != dwTransferred)
		printf_s("Partial SendCompletion requested [%d], sent [%d]\n", context->mWsaBuf.len, dwTransferred);
	else
		completionOk = true;

	break;


void Session::SendCompletion(DWORD transferred)
{
	TRACE_THIS;

	FastSpinlockGuard criticalSection(mSendBufferLock);

	mSendBuffer.Remove(transferred);

	mSendPendingCount--;
}
```
