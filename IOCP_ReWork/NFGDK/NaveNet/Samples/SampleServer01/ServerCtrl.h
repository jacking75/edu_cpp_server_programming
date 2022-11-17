#pragma once

#include "../../Server/NFConnectionManager.h"
#include "../../Server/NFServerCtrl.h"
#include "../../Server/NFPacketPool.h"

#include "TestConnection.h"


class Config;

class ServerCtrl : public NaveNetLib::NFServerCtrl
{
public:
	ServerCtrl(void);
	~ServerCtrl(void);
	
public:	
	void SetConfig(Config* pConfig);

	void StartServer();

	void EndServer();

	//void						Update();									// Process 처리 함수

	void						ShowServerInfo() override;							// 시작시 화면에 서버 정보 표시

	bool						CreateSession(unsigned long long sckListener);
	bool						ReleaseSession();

	//virtual void				InitializeCommand();

	// 사용자 오브젝트를 초기화 합니다. (윈도우설정이 종료됀 후에 호출)
	//virtual void				InitObject();
	// 사용자 오브젝트를 삭제합니다. (메인루프가 끊난후(EndProcess호출후) 호출)
	//virtual void				ReleaseObject();

public:
	void						UpdateInfo();
	void						SendMsg(char* strParam);


private:
	NaveNetLib::NFConnectionManager			m_NFConnectionManager;
	NaveNetLib::NFPacketPool		m_PacketPool;				// 메모리 처리 변수 
	TestConnection*					m_pLogin;					// Client List 변수

	unsigned int					m_iPrevTick;
	int								m_iMaxPacket;

	Config*							m_pConfig;
};