#include "stdafx.h"
#include "Actor.h"
#include "AIStateAttack.h"
#include "PCManager.h"
#include "Zone.h"

bool CAIStateAttack::CheckCondition(CActor const* actor_, CAIState* state_)
{
	return _CheckCondition(actor_);
}


bool CAIStateAttack::_CheckCondition(CActor const* actor_)
{
	CActor* actor = const_cast<CActor*>(actor_);
	CZone* zone = actor->GetZone();
	if (zone)
	{
		CActor* target = zone->FindPC(actor_->GetTargetID());
		if (target && target->IsValid())
		{
			if (target->GetActorType() == E_ACTOR::PC)
			{
				/// 거리 측정(vector로 변경예정)
				int gapX = static_cast<int>(abs(actor_->GetPosX() - target->GetPosX()));
				int gapY = static_cast<int>(abs(actor_->GetPosY() - target->GetPosY()));

				if (gapX < 50 && gapY < 50)
				{
					return true;
				}
			}
			else
			{
				// 다른 세력의 NPC 처리 등
			}
		}
	}
	return false;
}


void CAIStateAttack::Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_)
{
	m_pCurrentStateConditions = const_cast<LIST_FSM_CONDITON*>(conditions_);
	m_bIsPlaying = true;
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_ATTACK);
}


bool CAIStateAttack::Execute(CActor* actor_)
{
	if (!_CheckCondition(actor_))
	{
		m_bIsPlaying = false;
		__super::Execute(actor_);
	}

	return true;
}


void CAIStateAttack::Leave(CActor* actor_)
{
	actor_->SetActionState(E_ACTOR_STATE::ACTOR_RUN);
	m_bIsPlaying = false;
}


CAIStateAttack::CAIStateAttack(E_STATE nextState_)
	: CAIState(nextState_)
{

}


CAIStateAttack::~CAIStateAttack()
{

}