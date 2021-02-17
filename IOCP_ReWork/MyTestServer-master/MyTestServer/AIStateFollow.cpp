#include "stdafx.h"
#include "Actor.h"
#include "AIStateFollow.h"
#include "PCManager.h"
#include "Zone.h"
#include "sample.pb.h"
#include <math.h>
#include <Windows.h>



bool CAIStateFollow::CheckCondition(CActor const* actor_, CAIState* state_)
{
	if (state_->IsPlaying() && 
		state_->GetNextState() == GetNextState())
		return false;

	CActor* actor = const_cast<CActor*>(actor_);
	CZone* zone = actor->GetZone();
	if (zone)
	{
		CActor* target = zone->FindInDistPC(actor->GetPosCRef(), 400);
		if (target && target->IsValid())
		{
			actor->SetTargetID(target->GetActorID());
			return true;
		}
	}

	return false;
}


void CAIStateFollow::Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_)
{
	CZone* zone = actor_->GetZone();
	if (zone)
	{
		CActor* target = zone->FindPC(actor_->GetTargetID());
		if (target && target->IsValid())
		{
			m_pCurrentStateConditions = const_cast<LIST_FSM_CONDITON*>(conditions_);
			m_bIsPlaying = true;
			actor_->SetActionState(E_ACTOR_STATE::ACTOR_RUN);

			Vec3 destPos;
			if (!actor_->GetNaviMesh()->getNearPos(target->GetPosRef().ConvertPos(), destPos))
				return;

			m_targetPos = destPos;
			m_tickCount = ::GetTickCount();

			UnitNavi::S_PathNodeData move;
			move.From = actor_->GetPosRef().ConvertPos();
			move.To = destPos;
			actor_->GetNaviMesh()->CommandMove(move);
		}
	}
}


bool CAIStateFollow::Execute(CActor* actor_)
{
	__super::Execute(actor_);

	actor_->SetActionState(E_ACTOR_STATE::ACTOR_RUN);

	CZone* zone = actor_->GetZone();
	if (zone)
	{
		CActor* target = zone->FindPC(actor_->GetTargetID());
		if (target && target->IsValid())
		{
			if (::GetTickCount() - m_tickCount > 500)
			{
				Vec3 targetPos = target->GetPosRef().ConvertPos();
				if (m_targetPos.Distance(targetPos) > 50)
				{
					Vec3 destPos;
					if (!actor_->GetNaviMesh()->getNearPos(targetPos, destPos))
						return false;

					m_bIsPlaying = true;
					m_targetPos = destPos;

					UnitNavi::S_PathNodeData move;
					move.From = actor_->GetPosRef().ConvertPos();
					move.To = destPos;
					actor_->GetNaviMesh()->CommandMove(move);
				}
				m_tickCount = ::GetTickCount();
			}
		}
		else
		{
			m_bIsPlaying = false;
		}
	}
	actor_->GetNaviMesh()->Update();

	return true;
}


void CAIStateFollow::Leave(CActor* actor_)
{
	m_bIsPlaying = false;
}


CAIStateFollow::CAIStateFollow(E_STATE nextState_)
	: CAIState(nextState_)
{

}


CAIStateFollow::~CAIStateFollow()
{

}