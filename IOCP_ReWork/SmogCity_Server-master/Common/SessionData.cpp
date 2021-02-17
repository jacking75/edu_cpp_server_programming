
#include "SessionData.h"

CSessionData::CSessionData(WORD wid)
{
	m_wID				= wid;
	m_wGameSessionNum	= NOTREGISTED;
	m_bIsReference		= false;
	m_nPrevSize			= 0;

	ZeroMemory(&m_RecvBuffer, MAX_BUFFER);
	ZeroMemory(&m_ClientAddrInfo, sizeof(SOCKADDR_IN));
	ZeroMemory(&m_GamePlayerInfo, sizeof(GamePlayerInfo));

	m_IoContext = new stIoContext;
	m_Socket = INVALID_SOCKET;
	this->ClearIoContext();

	m_eClientState = e_Client_State::e_Login_State;
}

CSessionData::~CSessionData()
{
	delete m_IoContext;
	m_IoContext = nullptr;
}

VOID CSessionData::ClearSession()
{
	ZeroMemory(m_cName, MAX_NAME_LEN);

	ClearIoContext();

	//m_wGameSessionNum = NOTREGISTED;
	m_eClientState = e_Client_State::e_Login_State;
	m_bIsReference = false;
}

VOID CSessionData::SetClientAddr(SOCKADDR_IN* pclientAddr)
{
	ZeroMemory(&m_ClientAddrInfo, sizeof(SOCKADDR_IN));

	m_ClientAddrInfo.sin_family			= pclientAddr->sin_family;
	m_ClientAddrInfo.sin_port			= pclientAddr->sin_port;
	m_ClientAddrInfo.sin_addr.s_addr	= pclientAddr->sin_addr.s_addr;
}

VOID CSessionData::ClearIoContext()
{
	ZeroMemory(&m_ClientAddrInfo, sizeof(SOCKADDR_IN));

	ZeroMemory(&m_IoContext->m_cBuf, MAX_BUFFER);
	ZeroMemory(&m_IoContext->m_Wsabuf, sizeof(WSABUF));
	ZeroMemory(&m_IoContext->m_Overlapped, sizeof(WSAOVERLAPPED));

	closesocket(m_Socket);
	m_Socket = INVALID_SOCKET;

	m_IoContext->m_Wsabuf.buf = m_IoContext->m_cBuf;
	m_IoContext->m_Wsabuf.len = MAX_BUFFER;
}

const bool CSessionData::Send(void *ppacket)
{
	//m_Lock.Lock();				// 임계영역필요할까?
	{
		if (m_Socket == INVALID_SOCKET)
			return false;
		if (m_bIsReference == false)
			return false;

		int retval = 0;
		char *pbuf = reinterpret_cast<char *>(ppacket);

		ULONG size = static_cast<ULONG>(pbuf[0]);

		IoContext *sendCtx = new IoContext;
		ZeroMemory(sendCtx, sizeof(IoContext));
		sendCtx->m_eType = e_Callbacktype::e_Send;
		sendCtx->m_Wsabuf.buf = sendCtx->m_cBuf;
		sendCtx->m_Wsabuf.len = size;							// 첫번째 바이트에 사이즈값이 들어있다.
		//std::cout << "Send to ID : "<< static_cast<WORD>(GetID()) <<" Size : " << size << "\n";
		
		memcpy(sendCtx->m_cBuf, pbuf, size);
		ZeroMemory(&sendCtx->m_Overlapped, sizeof(WSAOVERLAPPED));

		retval = WSASend(m_Socket, &sendCtx->m_Wsabuf, 1, 0, 0, &sendCtx->m_Overlapped, NULL);
		if (retval == SOCKET_ERROR)
		{
			int errCode = WSAGetLastError();
			if (errCode != WSA_IO_PENDING)
			{
				//std::cout << "SendtoClient WSASend() Error! " << errCode << " \n";
				return false;
			}
		}
	}
	//m_Lock.UnLock();

	return true;
}

const bool CSessionData::SetPlayerInfo(WORD wid, bool isrespawn)
{
	m_UpdateLock.Lock();

	ZeroMemory(&m_GamePlayerInfo, sizeof(GamePlayerInfo));

	m_GamePlayerInfo.m_bIsLive = true;

	//m_GamePlayerInfo.m_wID = m_wID;
	if (isrespawn == false) {
		m_GamePlayerInfo.m_cInGameID = static_cast<BYTE>(wid);
		m_GamePlayerInfo.m_bIsChangingWeapon = false;
		m_GamePlayerInfo.m_cWeaponNum = WEAPON_MACHINEGUN;

		m_GamePlayerInfo.m_cMachinegunAmmo = MAX_MACHINEGUN_AMMO;
		m_GamePlayerInfo.m_cRocketgunAmmo = MAX_ROCKETGUN_AMMO;
	}

	// 아이디 값에 따라 플레이어 정보 설정
	switch (wid)
	{
	case 0: {
		m_GamePlayerInfo.m_bIsDown = true;

		m_GamePlayerInfo.m_fX = -16.f;
		m_GamePlayerInfo.m_fY = 12048.f;
		m_GamePlayerInfo.m_fZ = 129.f;

		m_GamePlayerInfo.m_fYaw = -90.f;
	} break;
	case 1: {
		m_GamePlayerInfo.m_bIsDown = false;

		m_GamePlayerInfo.m_fX = -30.f;
		m_GamePlayerInfo.m_fY = -11120.f;
		m_GamePlayerInfo.m_fZ = 29023.f;

		m_GamePlayerInfo.m_fYaw = 90.f;
	} break;
	case 2: {
		m_GamePlayerInfo.m_bIsDown = true;

		m_GamePlayerInfo.m_fX = -186.f;
		m_GamePlayerInfo.m_fY = 12048.f;
		m_GamePlayerInfo.m_fZ = 129.f;

		m_GamePlayerInfo.m_fYaw = -90.f;
	} break;
	case 3: {
		m_GamePlayerInfo.m_bIsDown = false;

		m_GamePlayerInfo.m_fX = -250.f;
		m_GamePlayerInfo.m_fY = -11120.f;
		m_GamePlayerInfo.m_fZ = 29023.f;

		m_GamePlayerInfo.m_fYaw = 90.f;
	} break;
	case 4: {
		m_GamePlayerInfo.m_bIsDown = true;

		m_GamePlayerInfo.m_fX = 144.f;
		m_GamePlayerInfo.m_fY = 12048.f;
		m_GamePlayerInfo.m_fZ = 129.f;

		m_GamePlayerInfo.m_fYaw = -90.f;
	} break;
	case 5: {
		m_GamePlayerInfo.m_bIsDown = false;

		m_GamePlayerInfo.m_fX = 210.f;
		m_GamePlayerInfo.m_fY = -11120.f;
		m_GamePlayerInfo.m_fZ = 29023.f;

		m_GamePlayerInfo.m_fYaw = 90.f;
	} break;
	case 6: {
		m_GamePlayerInfo.m_bIsDown = true;

		m_GamePlayerInfo.m_fX = -326.f;
		m_GamePlayerInfo.m_fY = 12048.f;
		m_GamePlayerInfo.m_fZ = 129.f;

		m_GamePlayerInfo.m_fYaw = -90.f;
	} break;
	case 7: {
		m_GamePlayerInfo.m_bIsDown = false;

		m_GamePlayerInfo.m_fX = -320.f;
		m_GamePlayerInfo.m_fY = -11120.f;
		m_GamePlayerInfo.m_fZ = 29023.f;

		m_GamePlayerInfo.m_fYaw = 90.f;
	} break;
	case 8: {
		m_GamePlayerInfo.m_bIsDown = true;

		m_GamePlayerInfo.m_fX = 294.f;
		m_GamePlayerInfo.m_fY = 12048.f;
		m_GamePlayerInfo.m_fZ = 129.f;

		m_GamePlayerInfo.m_fYaw = -90.f;
	} break;
	case 9: {
		m_GamePlayerInfo.m_bIsDown = false;

		m_GamePlayerInfo.m_fX = 370.f;
		m_GamePlayerInfo.m_fY = -11120.f;
		m_GamePlayerInfo.m_fZ = 29023.f;

		m_GamePlayerInfo.m_fYaw = 90.f;
	} break;
	default: break;
	}
	m_GamePlayerInfo.m_fLife = PLAYER_MAX_LIFE;

	m_GamePlayerInfo.m_fVX = 0.f;
	m_GamePlayerInfo.m_fVY = 0.f;
	m_GamePlayerInfo.m_fVZ = 0.f;

	m_UpdateLock.UnLock();

	return true;
}

const bool CSessionData::UpdatePlayerInfo()
{

	return true;
}