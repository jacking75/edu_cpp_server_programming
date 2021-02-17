#include "AIState.h"
#include "AIStateMachine.h"


void CAIState::SetFSM(CAIStateMachine* fsm_)
{
	m_pFSM = fsm_;
}


void CAIState::SetPlayState(bool bIs_) 
{ 
	m_bIsPlaying = bIs_; 
}


void CAIState::SetCurrentCondition(E_STATE_CONDITION condition_)
{
	m_currentConState = condition_;
}


LIST_FSM_CONDITON const* CAIState::GetConditions() 
{ 
	return m_pCurrentStateConditions; 
}


bool CAIState::CheckCondition(CActor const* actor_, CAIState* state_)
{
	return true;
}

void CAIState::Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_)
{
	// Don't call this
}

bool CAIState::Execute(CActor* actor_)
{
	if (actor_ && m_pCurrentStateConditions)
	{
		for (auto next : *m_pCurrentStateConditions)
		{
			if (next == this)
				continue;

			if (next->CheckCondition(actor_, this))
			{
				Leave(actor_);
				m_pFSM->SetNextState(next);
				return true;
			}
		}
	}
	return false;
}

void CAIState::Leave(CActor* actor_)
{
	// Don't call this
}

CAIState::CAIState(E_STATE nextState_)
	: m_nextState(nextState_)
{
}

CAIState::~CAIState()
{
}

