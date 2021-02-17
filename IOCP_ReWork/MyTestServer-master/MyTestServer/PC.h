#pragma once

#include <map>
#include "Actor.h"
#include "AIState.h"
#include "Define.h"
#include "PacketHeder.h"
#include "VirtualPoolObject.h"


class CSession;

///////////////////////////////////////////////////////////
// PC 클래스
///////////////////////////////////////////////////////////
class CPC :
	public CVirtualPoolObject,
	public CActor
{
public:
	struct _StInit
	{
		USER_ID userID;       ///< userID
		SERIAL_ID serialID;   ///< user의 serialID
	};

private:
	CSession* m_pSession;     ///< session
	USER_ID   m_userID;       ///< userID

public:
	/// userID를 가지고 온다
	inline USER_ID GetUserID() const { return m_userID; }

	/// session을 가지고 온다
	inline CSession* GetSession() { return m_pSession; }

	/// session을 등록한다.
	void SetSession(CSession* session_);

	/// 오브젝트 풀에서 가져올 때 호출
	virtual bool Leave(void* initParam_) override;

	/// 오브젝트 풀에 들어갈 때 호출
	virtual bool Enter() override;

	/// 업데이트
	virtual void Update() override;

	/// 현재 AI상태에 대한 동작 유무
	virtual void SetAIPlayState(bool bIs_) override;

	/// 유효한 actor인가
	virtual bool IsValid() const override;


public:
	CPC();
	virtual ~CPC();
};