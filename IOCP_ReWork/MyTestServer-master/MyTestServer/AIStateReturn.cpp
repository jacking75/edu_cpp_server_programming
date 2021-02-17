#include "stdafx.h"
#include "AIStateReturn.h"
#include "Actor.h"
#include "AIStateMachine.h"
#include "PC.h"
#include "Zone.h"

bool CAIStateReturn::CheckCondition(CActor const* actor_, CAIState* state_)
{
	if (state_->IsPlaying() &&
		state_->GetNextState() == GetNextState())
		return false;

	CZone* zone = actor_->GetZone();
	if (zone)
	{
		CActor* target = zone->FindPC(actor_->GetTargetID());
		if (target && target->IsValid())
		{
			if (target->GetActorType() == E_ACTOR::PC)
			{
				float dist = const_cast<CActor*>(actor_)->GetPosRef().Distance(target->GetPosRef());
				if (dist > 600)
				{
					return true;
				}

				if (target->GetActionState() == E_ACTOR_STATE::ACTOR_DEAD)
				{
					return true;
				}
			}
			return false;
		}
		
	}
	return true;
}


void CAIStateReturn::Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_)
{

	m_pCurrentStateConditions = const_cast<LIST_FSM_CONDITON*>(conditions_);
	m_bIsPlaying = true;
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_RETURN);
	
	Vec3 targetPos;
	if (!actor_->GetNaviMesh()->getNearPos(actor_->GetSpawnPosRef().ConvertPos(), targetPos))
		return;

	UnitNavi::S_PathNodeData move;
	move.From = actor_->GetPosRef().ConvertPos();
	move.To = targetPos;
	actor_->GetNaviMesh()->CommandMove(move);
}


bool CAIStateReturn::Execute(CActor* actor_)
{
 	__super::Execute(actor_);

	actor_->GetNaviMesh()->Update();
	
	return true;
}


void CAIStateReturn::Leave(CActor* actor_)
{
	actor_->SetTargetID(INVALID_SERIAL_ID);
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_STAND);
	m_bIsPlaying = false;
}


CAIStateReturn::CAIStateReturn(E_STATE nextState_)
	: CAIState(nextState_)
{

}


CAIStateReturn::~CAIStateReturn()
{

}