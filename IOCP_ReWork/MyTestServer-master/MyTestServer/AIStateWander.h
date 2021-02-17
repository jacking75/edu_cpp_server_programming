#pragma once
#include "AIState.h"


///////////////////////////////////////////////////////////
// AIState 돌아다니기
// (다음 상태)
///////////////////////////////////////////////////////////
class CAIStateWander :
	public CAIState
{
private:
	float TargetX;  ///< 목표의 x 좌표
	float TargetY;  ///< 목표의 y 좌표

public:
	/// AI 진입 조건에 맞는지 체크
	virtual bool CheckCondition(CActor const* actor_, CAIState* state_) override;

	/// AI 진입
	virtual void Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_) override;

	/// AI 수행
	virtual bool Execute(CActor* actor_) override;

	/// AI 수행 후 퇴장
	virtual void Leave(CActor* actor_) override;

public:
	CAIStateWander(E_STATE nextState_);
	virtual ~CAIStateWander();
};