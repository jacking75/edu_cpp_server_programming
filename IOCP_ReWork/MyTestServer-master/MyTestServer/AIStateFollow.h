#pragma once
#include "AIState.h"


///////////////////////////////////////////////////////////
// AIState 따라가기
// (다음 상태)
///////////////////////////////////////////////////////////
class CAIStateFollow :
	public CAIState
{
private:
	Vec3 m_targetPos;   ///< target의 좌표
	double m_tickCount; ///<  목표 갱신을 위한 틱

private:
	/// AI 진입 조건에 맞는지 체크
	bool _CheckCondition(CActor const* actor_);

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
	CAIStateFollow(E_STATE nextState_);
	virtual ~CAIStateFollow();
};