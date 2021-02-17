#include "stdafx.h"
#include "AIState.h"
#include "AIStateMachine.h"
#include "Define.h"


CAIStateMachine::CAIStateMachine(CActor* actor_)
	: m_pActor(actor_)
{
	if (actor_ == NULL)
	{
		// 로그
		return;
	}
}


CAIStateMachine::~CAIStateMachine()
{
	for (auto it : m_stateMachine)
	{	
		for (LIST_FSM_CONDITON::iterator itc = it.second->begin(); itc != it.second->end(); ++itc)
		{
			SAFE_DELETE((*itc));
		}
		SAFE_DELETE(it.second);
	}
}


bool CAIStateMachine::RegistState(E_STATE state_, E_STATE_CONDITION condiiton_, CAIState* stateMachine_)
{
	if (stateMachine_)
	{
		stateMachine_->SetFSM(this);
		stateMachine_->SetCurrentCondition(condiiton_);
		auto it = m_stateMachine.find(state_);
		if (it != m_stateMachine.end())
		{
			LIST_FSM_CONDITON* conditions = (*it).second;
			for (LIST_FSM_CONDITON::iterator itc = conditions->begin(); itc != conditions->end(); ++itc)
			{
				if ((*itc)->GetStateCondition() == condiiton_)
					continue;

				conditions->push_back(stateMachine_);
				return true;	
			}
		}
		else
		{
			LIST_FSM_CONDITON* condition = new LIST_FSM_CONDITON;
			if (condition == NULL)
			{
				// 로그
				return false;
			}
			condition->push_back(stateMachine_);
			m_stateMachine.insert(std::make_pair(state_, condition));
			return true;		
		}
	}
	return false;
}


bool CAIStateMachine::SetCurrentState(E_STATE state_, E_STATE_CONDITION condition_)
{
	auto it = m_stateMachine.find(state_);
	if (it == m_stateMachine.end())
	{
		return false;
	}

	for (auto itc : *(*it).second)
	{
		if (itc->GetStateCondition() == condition_)
		{
			m_pCurrentState = itc;
			m_pCurrentState->Enter(m_pActor, (*it).second);

			return true;
		}
	}

	return false;
}


bool CAIStateMachine::SetNextState(CAIState* state_)
{
	auto it = m_stateMachine.find(state_->GetNextState());
	if (it != m_stateMachine.end())
	{
		state_->Enter(m_pActor, (*it).second);
		m_pCurrentState = state_;
		return true;
	}
	return false;
}


bool CAIStateMachine::RemoveState(E_STATE state_, E_STATE_CONDITION condition_)
{
	auto it = m_stateMachine.find(state_);
	if (it == m_stateMachine.end())
	{
		return false;
	}

	for (auto itc : *(*it).second)
	{
		if (itc->GetStateCondition() == condition_)
		{
			SAFE_DELETE(itc);
			(*it).second->remove(itc);

			if ((*it).second->empty())
			{
				SAFE_DELETE((*it).second);   //< new로 생성된 컨테이너 삭제
				m_stateMachine.erase(state_);
			}

			return true;
		}
	}

	return false;
}


void CAIStateMachine::SetCurrentPlayState(bool bIs_)
{
	if (m_pCurrentState)
	{
		m_pCurrentState->SetPlayState(bIs_);
	}
	else
	{
		// 로그
	}	
}


void CAIStateMachine::PushEvent(_StAIStateEvent const& condition_)
{
	m_eventQueue.push(condition_);
}


void CAIStateMachine::Update()
{
	if (m_pCurrentState)
	{
		if (m_eventQueue.empty())
		{
			m_pCurrentState->Execute(m_pActor);
		}
		else
		{
			_StAIStateEvent event = m_eventQueue.front();
			m_eventQueue.pop();
			auto it = m_stateMachine.find(event.state);
			if (it != m_stateMachine.end())
			{
				LIST_FSM_CONDITON* conditionMap = (*it).second;

				for (auto itc : *(*it).second)
				{
					if (itc->GetStateCondition() == event.condition)
					{
						m_pCurrentState->Leave(m_pActor);
						itc->Enter(m_pActor, (*it).second);
						m_pCurrentState = itc;
					}
				}	
			}
		}
	}
}
