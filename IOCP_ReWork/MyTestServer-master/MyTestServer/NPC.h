#pragma once

#include "Actor.h"
#include "AIState.h"
#include <map>


class CAIStateMachine;

///////////////////////////////////////////////////////////
// NPC 클래스
///////////////////////////////////////////////////////////
class CNPC :
	public CActor
{
private:
	CAIStateMachine* m_stateMachine;    ///< FSM 클래스


public:
	///  AI 업데이트
	virtual void Update() override;

	/// 현재 AI상태에 대한 동작 유무
	virtual void SetAIPlayState(bool bIs_) override;

	/// 유효한 actor인가
	virtual bool IsValid() const override;
	

public:
	CNPC();
	virtual ~CNPC();
};