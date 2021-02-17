#include "stdafx.h"
#include "AIStateWander.h"
#include "Actor.h"
#include "AIStateMachine.h"


bool CAIStateWander::CheckCondition(CActor const* actor_, CAIState* state_)
{
	if (state_->IsPlaying() &&
		state_->GetNextState() == GetNextState())
		return false;

	// 확률적으로 true 시킨다.
	// 일단
	if (rand() % 30 == 0)
	{
		return true;
	}
	return false;
}


void CAIStateWander::Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_)
{
	// 일단
	TargetX = actor_->GetSpawnPosX() + (rand() % 500) - 250;
	TargetY = actor_->GetSpawnPosY() + (rand() % 500) - 250;

	Vec3 targetPos(TargetX, actor_->GetSpawnPosZ(), TargetY);
	Vec3 destPos;

	if (!actor_->GetNaviMesh()->getNearPos(targetPos, destPos))
		return;

	m_pCurrentStateConditions = const_cast<LIST_FSM_CONDITON*>(conditions_);
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_WALK);

	m_bIsPlaying = true;

	UnitNavi::S_PathNodeData move;
	move.From = actor_->GetPosRef().ConvertPos();
	move.To = destPos;
	actor_->GetNaviMesh()->CommandMove(move);
	
}


bool CAIStateWander::Execute(CActor* actor_)
{
	__super::Execute(actor_);

	actor_->GetNaviMesh()->Update();
	
	return true;
}


void CAIStateWander::Leave(CActor* actor_)
{
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_STAND);
	m_bIsPlaying = false;
}


CAIStateWander::CAIStateWander(E_STATE nextState_)
	: CAIState(nextState_)
{

}


CAIStateWander::~CAIStateWander()
{

}