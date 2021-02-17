#include "stdafx.h"
#include "Actor.h"
#include "AIStateIdle.h"
#include "AIStateMachine.h"


bool CAIStateIdle::CheckCondition(CActor const* actor_, CAIState* state_)
{
	if (state_->IsPlaying())
		return false;
	return true;
}


void CAIStateIdle::Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_)
{
	m_pCurrentStateConditions = const_cast<LIST_FSM_CONDITON*>(conditions_);
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_STAND);
}


bool CAIStateIdle::Execute(CActor* actor_)
{
	__super::Execute(actor_);
	return true;

}


void CAIStateIdle::Leave(CActor* actor_)
{
}


CAIStateIdle::CAIStateIdle(E_STATE nextState_)
	: CAIState(nextState_)
{

}


CAIStateIdle::~CAIStateIdle()
{

}