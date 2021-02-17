
#include <WinSock2.h>
#include "ServerManager.h"
#include "../Network/InitialNetwork.h"
#include "../Common/Protocol.h"

using std::cout;

CServerManager::CServerManager()
{
	/*m_hSendEvent = INVALID_HANDLE_VALUE;
	m_hStopEvent = INVALID_HANDLE_VALUE;*/
}

CServerManager::~CServerManager()
{
	/*if (this->StopServer() != true)
		cout << "ERROR : [return FALSE] ServiceMain::~ServiceMain\n";*/

	/*SAFE_CLOSE_HANDLE(m_hSendEvent);
	SAFE_CLOSE_HANDLE(m_hStopEvent);*/
}

const bool CServerManager::InitialAllData()
{
	if (this->InitialNetwork() != true)
	{
		cout << "InitialAllData() Initialize Fail : Network\n";
		return false;
	}

	cout << "InitialAllData() Initialize OK : Network\n";

	if (this->InitialTimer() != true)
	{
		cout << "InitialAllData() Initialize Fail : Timer\n";
		return false;
	}

	cout << "InitialAllData() Initialize OK : Timer\n";

	if (this->InitialDB() != true)
	{
		cout << "InitialAllData() Initialize Fail : DB\n";
		return false;
	}

	cout << "InitialAllData() Initialize OK : DB\n";

	if (this->InitialThread() != true)
	{
		cout << "InitialAllData() Initialize Fail : Thread\n";
		return false;
	}

	cout << "InitialAllData() Initialize OK : Thread\n";

	return true;
}

// 윈속 초기화 및 IOCP객체 생성
const bool CServerManager::InitialNetwork()
{
	if ( START_NETWORK() != true )
		return false;

	if ( m_IOCP.StartIOCP() != true )
		return false;

	return true;
}

// 스레드들 초기화 및 실행
const bool CServerManager::InitialThread()
{
	m_AcceptThread.Initialize(this, _AcceptThread);
	if (m_AcceptThread.StartThread() != true)
		return false;

	m_WorkerThread.Initialize(NUM_THREAD, this, _WorkerThread);
	if (m_WorkerThread.StartMultiThread() != true)
		return false;

	/*m_SendThread.Initialize(this, _SendThread);
	if (m_SendThread.StartThread() != true)
		return false;*/

	m_TimerThread.Initialize(this, _TimerThread);
	if (m_TimerThread.StartThread() != true)
		return false;

	return true;
}

const bool CServerManager::InitialTimer()
{
	if (m_Timer.Initialize(m_IOCP.GetCompletionPort()) != true)
		return false;

	return true;
}

const bool CServerManager::InitialDB()
{
	if (m_DB.InitalizeDB() == false)
		return false;

	if (m_DB.ConnectDB((SQLWCHAR*)L"Smog_City_Server", (SQLWCHAR*)L"TJ", (SQLWCHAR*)L"102030") == false)
		return false;

	return true;
}

const bool CServerManager::ReleaseAllData()
{

	return true;
}

const bool CServerManager::StartServer()
{
	cout << "StartServer() Start...\n";

	if (this->InitialAllData() != true)
		return false;

	cout << "StartServer() Initialize : Success\n";

	return true;
}

const bool CServerManager::StopServer()
{
	if (this->m_AcceptThread.StopThread() != true)
	{
		cout << "StopServer() Fail : AcceptThread\n";
		return false;
	}

	cout << "StopServer() OK : AcceptThread\n";

	if (this->m_DB.DisconnectDB() != true)
	{
		cout << "StopServer() Fail : DB\n";
		return false;
	}
	this->m_DB.ClearDB();
	cout << "StopServer() OK : DB\n";

	if (this->m_WorkerThread.StopMultiThread() != true)
	{
		cout << "StopServer() Fail : WorkerThread\n";
		return false;
	}

	cout << "StopServer() Release OK : WorkerThread\n";

	/*if (this->m_SendThread.StopThread() != true)
	{
		cout << "StopServer() Fail : SendThread\n";
		return false;
	}

	cout << "StopServer() OK : SendThread\n";*/

	if (this->m_TimerThread.StopThread() != true)
	{
		cout << "StopServer() Fail : TimerThread\n";
		return false;
	}

	cout << "StopServer() OK : TimerThread\n";

	if (END_NETWORK() != true)
	{
		cout << "StopServer() Fail : END_NETWORK\n";
		return false;
	}

	cout << "StopServer() OK : END_NETWORK\n";

	if (this->ReleaseAllData() != true)
		return false;

	cout << "StopServer() OK : ReleaseAllData\n";

	return true;
}

const UINT CServerManager::_AcceptThread(LPVOID lpParam)
{
	CServerManager* pthis = (CServerManager*)lpParam;
	if ( !pthis)
		return false;

	pthis->AcceptThread();

	return true;
}

const UINT CServerManager::_WorkerThread(LPVOID lpParam)
{
	CServerManager* pthis = (CServerManager*)lpParam;
	if ( !pthis)
		return false;

	pthis->WorkerThread();

	return true;
}

const UINT CServerManager::_SendThread(LPVOID lpParam)
{
	CServerManager* pthis = (CServerManager*)lpParam;
	if (!pthis)
		return false;

	pthis->SendThread();

	return true;
}

const UINT CServerManager::_TimerThread(LPVOID lpParam)
{
	CServerManager* pthis = (CServerManager*)lpParam;
	if (!pthis)
		return false;

	pthis->TimerThread();

	return true;
}

const bool CServerManager::AcceptThread()
{
	SOCKET		serverSocket = INVALID_SOCKET;
	SOCKET		clientSocket = INVALID_SOCKET;
	SOCKADDR_IN	serverAddr;
	SOCKADDR_IN	clientAddr;

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	ZeroMemory(&clientAddr, sizeof(clientAddr));

	// WSASocketW가 WSASocketA보다 최신
	serverSocket = WSASocketW(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (serverSocket == INVALID_SOCKET) {
		cout << "[WSASocketW() Error!]\n";
		return false;
	}
	// bind() 과정
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_port			= htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	if ( bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR )
		return false;

	// listen()
	if ( listen(serverSocket, SOMAXCONN) == SOCKET_ERROR )
		return false;

	int addrLen = sizeof(SOCKADDR_IN);

	while ( m_AcceptThread.IsRun() )
	{
		DWORD flags		= 0;

		clientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);

		if ( clientSocket == INVALID_SOCKET )
		{
			cout << "AcceptThread accept() Error " << WSAGetLastError() << "\n";
			return 0;
		}

		// 논블락 적용 // 필요한가?
		DWORD nonblock = 1;
		if (ioctlsocket(clientSocket, FIONBIO, &nonblock) == SOCKET_ERROR) {
			cout << "AcceptThread ioctlsocket Error" << WSAGetLastError() << "\n";
			continue;
		}

		// 커널의 수신버퍼와 송신버퍼의 크기 조절 // 
		/*int	nZero = 0;
		if ( SOCKET_ERROR == setsockopt(hClntSock, SOL_SOCKET, SO_RCVBUF, (const TCHAR*)&nZero, sizeof(nZero)) )
			continue;

		if ( SOCKET_ERROR == setsockopt(hClntSock, SOL_SOCKET, SO_SNDBUF, (const TCHAR*)&nZero, sizeof(nZero)) )
			continue;*/

		CSessionData *psession = m_SessionPool.CreateSession();
		if (psession == nullptr)
			continue;
		psession->m_Socket = clientSocket;
		psession->SetClientAddr(&clientAddr);

		if (m_SessionPool.InsertSession(psession) != true)					// insert 해줄 필요없을거같다.	// 맵에 추가해줘야한다.
			continue;

		if (m_IOCP.RegisterClient(clientSocket, psession) != true)
		{
			cout << "RegisterClient() Error " << WSAGetLastError() << "\n";
			m_SessionPool.DeleteSession(psession);
			continue;
		}
		cout << "ID : " << static_cast<int>(psession->GetID()) << "로 접속 \n";
		psession->m_IoContext->m_eType = e_Recv;
		int retval = WSARecv(clientSocket, &(psession->m_IoContext->m_Wsabuf), 1, 0, &flags, &(psession->m_IoContext->m_Overlapped), NULL);

		if (retval == SOCKET_ERROR)
		{
			int errCode = WSAGetLastError();
			if (errCode != WSA_IO_PENDING )
			{
				err_display("AcceptThread WSARecv() Error", errCode);
				//cout << "AcceptThread WSARecv() Error " << errCode << " \n";
				return false;
			}
		}
	}

	return true;}

/*
 *	멀티 스레드를 사용한다. 따라서 멤버 변수에 접근하면 꼭 Lock 을 걸어둔다
*/
const bool CServerManager::WorkerThread()
{
	DWORD				ioByte = 0;
	DWORD				retval = 0;
	CSessionData		*psession = nullptr;
	IoContext			*poverlapped = nullptr;
	
	while ( m_WorkerThread.IsRun() == true )
	{
		ioByte = 0;
		retval = 0;
		psession = nullptr;
		poverlapped = nullptr;

		if ( false == GetQueuedCompletionStatus(m_IOCP.GetCompletionPort(), &ioByte,
												reinterpret_cast<PULONG_PTR>(&psession),
												reinterpret_cast<LPOVERLAPPED *>(&poverlapped), INFINITE))
			retval = WSAGetLastError();

		// 오류가 없으면
		if (retval == 0)
		{
			e_Callbacktype etype = poverlapped->m_eType;

			if (etype == e_Callbacktype::e_Timer_Update) {
				CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(poverlapped->m_Target);

				pgameSession->UpdateGame(&m_Timer);
				if (pgameSession->IsRun() == true)
					m_Timer.AddTimer(NOW + TIMER_UPDATE_GAME, e_Callbacktype::e_Timer_Update, nullptr, poverlapped->m_Target);

				SendSC_Game_Info(pgameSession, nullptr, true);

				delete poverlapped;
			}
			else if (etype == e_Callbacktype::e_Timer_Respawn_Object) {
				CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(poverlapped->m_Target);
				
				GameObjectInfo* objectInfo = pgameSession->RegenObject(poverlapped->m_SubTarget);
				if(objectInfo != nullptr)
					SendSC_Put_Object(objectInfo, poverlapped->m_Target);

				delete poverlapped;
			}
			else if (etype == e_Callbacktype::e_Timer_Round_Start) {
				CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(poverlapped->m_Target);
				for (auto object : pgameSession->m_mapGameObjects) {
					SC_PACKET_REMOVE_OBJECT packet;
					packet.id = (object.first);
					packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
					packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

					if (m_SessionPool.BroadcastSendInGame(poverlapped->m_Target, &packet) == false)
						break;
				}
				pgameSession->RoundEnd();
				// 플레이어들의 위치를 제대로 시작위치로 고정시켜줘야 함
				for (int i = 0; i < MAX_PLAYER; ++i) {
					if (pgameSession->m_Sessions[i] == nullptr) continue;
					bool isneedswap = false;
					if (pgameSession->m_Sessions[i]->m_GamePlayerInfo.m_cWeaponNum != WEAPON_MACHINEGUN)
						isneedswap = true;
					pgameSession->m_Sessions[i]->SetPlayerInfo(i, false);
					if (isneedswap)
						SendSC_Swaped_Weapon(pgameSession->m_Sessions[i]);
					SendSC_Update(pgameSession->m_Sessions[i]);
					SC_PACKET_RESPAWN_PLAYER packet;
					ZeroMemory(&packet, sizeof(packet));
					packet.id = (pgameSession->m_Sessions[i]->GetID());
					packet.size = sizeof(SC_PACKET_RESPAWN_PLAYER);
					packet.type = e_SC_PACKET_TYPE::e_Respawn_Player;

					pgameSession->m_Sessions[i]->Send(&packet);
					
					SendSC_Ammo_Update(pgameSession->m_Sessions[i]);
				}
				for (int i = 0; i < BASIC_OBJECT_NUM; ++i) 
					SendSC_Put_Object(&pgameSession->m_GameObject[i]->m_GameObjectInfo, poverlapped->m_Target);

				SC_PACKET_ROUND_RESULT packet;
				packet.size = sizeof(SC_PACKET_ROUND_RESULT);
				packet.type = e_SC_PACKET_TYPE::e_Round_Start;

				pgameSession->BroadcastSendInGame(&packet);

				delete poverlapped;
			}
			// 시간 값을 애니메이션에 맞춰서 바꿔주자!
			//else if (etype == e_Callbacktype::e_Timer_Swap_Weapon) {
			//	psession->m_GamePlayerInfo.m_cWeaponNum = poverlapped->m_Target;
			//	psession->m_GamePlayerInfo.m_bIsChangingWeapon = false;

			//	//SendSC_Update

			//	delete poverlapped;
			//}

			if (psession == nullptr)				// 여기로 오는 경우가 있나?
				continue;

			if (ioByte == 0)	// 클라이언트가 접속을 끊었다
			{
				//this->RecvCS_Logout(psession);
				cout << "WorkerThread LogOut ID : " << static_cast<int>(psession->GetID()) << "\n";
				if (psession->m_eClientState == e_Client_State::e_Game_State) {
					psession->m_CharacterInfo.m_nGameAmount += 1;
					psession->m_CharacterInfo.m_nGameDefeat += 1;
				}
				DisconnectPlayer(psession);
				continue;
			}
			//cout << ioByte << "바이트 받음\n";
			

			switch (etype)
			{
			case e_Callbacktype::e_Recv:
			{
				int packetSize = 0;
				char *pbuf = psession->m_IoContext->m_cBuf;

				if (psession->m_nPrevSize != 0)			// 이전 받았던 데이터에서 사이즈를 받아온다.
					packetSize = psession->m_RecvBuffer[0];
				while (ioByte > 0)
				{
					if (packetSize == 0)
						packetSize = pbuf[0];
					UINT required = packetSize - psession->m_nPrevSize;		// 패킷을 완성하는데 필요한 바이트 수
					if (ioByte < required) // 완성하지 못하면
					{
						memcpy(psession->m_RecvBuffer + psession->m_nPrevSize, pbuf, ioByte);
						psession->m_nPrevSize += ioByte;
						break;
					}
					else
					{
						memcpy(psession->m_RecvBuffer + psession->m_nPrevSize, pbuf, required);
						// RecvBuf에 쌓인 데이터 처리
						PacketProcess(psession);

						// 다음 패킷을 조립하기 위한 준비
						psession->m_nPrevSize	= 0;
						ioByte					-= required;
						pbuf					+= required;
						packetSize				= 0;
					}
				}

				DWORD flags = 0;			// recv를 꼭 다시 호출해줘야한다.
				ZeroMemory(&psession->m_IoContext->m_Overlapped, sizeof(WSAOVERLAPPED));

				retval = WSARecv(psession->m_Socket, &(psession->m_IoContext->m_Wsabuf), 1, NULL, &flags, &(psession->m_IoContext->m_Overlapped), NULL);

				if (retval == SOCKET_ERROR)
				{
					int errCode = WSAGetLastError();
					if (errCode != WSA_IO_PENDING)
					{
						cout << "WorkerThread WSARecv() Error " << errCode << " \n";
						return false;
					}
				}
			} break;
			case e_Callbacktype::e_Send:
			{
				// Send일때 new해준 IoContext 할당해체 해주자.
				delete poverlapped;
			} break;
			// 여기부터 타이머 콜백
			{
				case e_Callbacktype::e_Timer_Start:
				{
					// 동적할당해서 호출하므로 꼭 딜리트해주자
					delete poverlapped;
				} break;
				case e_Callbacktype::e_Timer_Respawn_Player:
				{
					delete poverlapped;

					if (psession->GetReference() == true) {
						bool isneedswap = false;
						if (psession->m_GamePlayerInfo.m_cWeaponNum != WEAPON_MACHINEGUN)
							isneedswap = true;
						psession->SetPlayerInfo(psession->m_GamePlayerInfo.m_cInGameID, false);
						if(isneedswap)
							SendSC_Swaped_Weapon(psession);
						SendSC_Update(psession);
						//SendSC_Swaped_Weapon(psession);
						SendSC_Ammo_Update(psession);
						psession->m_GamePlayerInfo.m_bIsLive = true;
						SendSC_Respawn_Player(psession->GetID(), psession->m_wGameSessionNum);
					}
				} break;
				case e_Callbacktype::e_Timer_Swap_Weapon:
				{
					psession->m_GamePlayerInfo.m_cWeaponNum = poverlapped->m_Target;
					psession->m_GamePlayerInfo.m_bIsChangingWeapon = false;

					SendSC_Swaped_Weapon(psession);

					SendSC_Ammo_Update(psession);

					delete poverlapped;
				} break;
				case e_Callbacktype::e_Timer_Reload:
				{
					psession->m_GamePlayerInfo.m_bIsReloading = false;
					switch (psession->m_GamePlayerInfo.m_cWeaponNum)
					{
					case WEAPON_MACHINEGUN:
					{
						psession->m_GamePlayerInfo.m_cMachinegunAmmo = MAX_MACHINEGUN_AMMO;
					} break;
					case WEAPON_ROCKETGUN:
					{
						psession->m_GamePlayerInfo.m_cRocketgunAmmo = MAX_ROCKETGUN_AMMO;
					} break;
					}

					SendSC_Ammo_Update(psession);

					delete poverlapped;
				} break;
			}
			default: continue;
			}
		}
		else  // 오류들 처리
		{
			// INFINITY값을 주었으므로 여기에 들어올 일은 없다. 아마도..?
			if (WAIT_TIMEOUT == retval)
			{
			}
			// ERROR_NETNAME_DELETED 에러가 나는 경우는 상대방이 정상종료(closesocket, shotdown 함수를 이용한 종료)
			// 으로 인해 종료하였을 경우 0바이트를 보내는데 위의 함수를 호출하지 않고 종료를 한 상태에서 read, write를 시도하면 이 오류가 나온다.
			// 접속이 끊겼다는 것이니까 연결을 끊어주자
			else if (ERROR_NETNAME_DELETED == retval)
			{
				//cout << "WorkerThread ERROR CODE : ERROR_NETNAME_DELETED\n";
				cout << "ID : " << static_cast<int>(psession->GetID()) << " LogOut!\n";
				if (psession->m_eClientState == e_Client_State::e_Game_State) {
					psession->m_CharacterInfo.m_nGameAmount += 1;
					psession->m_CharacterInfo.m_nGameDefeat += 1;
				}
				DisconnectPlayer(psession);
			}
			else 
			{
				cout << "GetQueuedCompletionStatus unknown Error " << WSAGetLastError() << "\n";
				DisconnectPlayer(psession);
			}

			continue;
		}
	}

	return true;
}

const bool CServerManager::SendThread()
{
	//DWORD	dwSendBytes = 0;
	//DWORD	dwFlags = 0;

	//while ( m_SendThread.IsRun() )
	//{
	//	//cout << "Send!\n";
	//	if ( m_SendCtx != NULL )
	//	{
	//		int nReturn = ::WSASend(m_SendCtx->m_Socket, &m_SendCtx->m_IoContext->m_Wsabuf, 1, &dwSendBytes, dwFlags, &(m_SendCtx->m_IoContext->m_Overlapped), NULL);

	//		if ( nReturn == SOCKET_ERROR )
	//		{
	//			if ( WSAGetLastError() != ERROR_IO_PENDING )
	//			{
	//				// 에러처리
	//			}
	//		}

	//		m_SendCtx = NULL;
	//	}
	//}

	return true;
}

const bool CServerManager::TimerThread()
{
	while (m_TimerThread.IsRun())
	{
		Sleep(10);

		m_Timer.ProcessTimer();
	}

	return true;
}

const bool CServerManager::PacketProcess(CSessionData* psession)
{
	bool breturn = true;

	PACKET_HEADER *header = reinterpret_cast<PACKET_HEADER *>(psession->m_RecvBuffer);
	
	BYTE size = header->size;
	BYTE type = header->type;

	// 여기서 무슨 패킷의 타입의 따라 처리해주고 그에 맞는 샌드를 해주자
	switch ( type )
	{
	case e_CS_PACKET_TYPE::e_Login:
	{
		this->RecvCS_Login(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Signup:
	{
		this->RecvCS_Signup(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Join:
	{
		this->RecvCS_Join(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Access_Game_Ok:
	{
		this->RecvCS_Access_Game_Ok(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Update:
	{
		this->RecvCS_Update(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Shoot:
	{
		this->RecvCS_Shoot(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Logout:
	{
		this->RecvCS_Logout(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Collision:
	{
		this->RecvCS_Collision(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Back_To_Lobby:
	{
		this->RecvCS_Back_To_Lobby(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Swap_Weapon:
	{
		this->RecvCS_Swap_Weapon(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Reload:
	{
		this->RecvCS_Reload(psession);
	} break;
	case e_CS_PACKET_TYPE::e_Back_To_MainMenu:
	{
		this->RecvCS_Back_To_MainMenu(psession);
	} break;
	default					:	breturn = false;							break;
	}

	//m_SessionPool.UpdatePlayersInfo();

	return breturn;
}

const bool CServerManager::DisconnectPlayer(CSessionData* psession)
{
	if(psession->m_eClientState != e_Client_State::e_Login_State)
		m_DB.SaveCharacterInfoToDB(psession);

	WORD wgameSessionNum = psession->m_wGameSessionNum;
	if (wgameSessionNum == NOTREGISTED)
	{
		m_SessionPool.DeleteSession(psession);
		this->SendSC_Remove_Player(psession);

		return true;
	}
	CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(wgameSessionNum);
	e_Client_State clientState = psession->m_eClientState;
	GamePlayerInfo* pplayerInfo = &psession->m_GamePlayerInfo;

	// 거점에 들어와있는 상태에서 나가면 갯수를 줄여주자.
	if (clientState == e_Client_State::e_Game_State && pplayerInfo->m_bIsInMainBase == true)
	{
		if (pplayerInfo->m_bIsDown == true)
			pgameSession->m_GameInfo.m_nDownPlayerCount -= 1;
		else
			pgameSession->m_GameInfo.m_nUpPlayerCount -= 1;
	}

	TCHAR name[MAX_NAME_LEN] {};
	lstrcpy(name, psession->m_cName);

	m_SessionPool.DeleteSession(psession);
	this->SendSC_Remove_Player(psession);

	if (clientState == e_Client_State::e_Game_State) {
		SendSC_InGame_Log(name, INGAME_LOG_LEAVE, nullptr, wgameSessionNum);

		SC_PACKET_GAME_SESSION_INFO packet;
		int size = sizeof(SC_PACKET_GAME_SESSION_INFO);
		packet.size = size;
		packet.type = e_SC_PACKET_TYPE::e_Game_Session_Info;
		packet.isstart = pgameSession->IsRun();
		packet.roomnum = (wgameSessionNum);
		packet.currentnum = pgameSession->GetPlayerNum();

		m_SessionPool.BroadcastSend(&packet);
	}

	return true;
}