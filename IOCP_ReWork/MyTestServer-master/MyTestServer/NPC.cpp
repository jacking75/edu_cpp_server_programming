#pragma once
#include "NPC.h"
#include "Actor.h"
#include "AIStateMachine.h"
#include "AIStateIdle.h"
#include "AIStateReturn.h"
#include "AIStateAttack.h"
#include "AIStateFollow.h"
#include "AIStateWander.h"
#include "AIStateHit.h"
#include "Define.h"
#ifdef __TBB_malloc_proxy
#include "tbb/tbbmalloc_proxy.h"
#else
#include "tcmalloc.h"
#endif


CNPC::CNPC()
	: CActor(E_ACTOR::NPC)
{
	m_stateMachine = ALLOCATE_POOL_NEW(m_stateMachine, CAIStateMachine, this);
	if (m_stateMachine == NULL)
	{
		// 로그
		return;
	}

	/// AI의 패턴을 등록한다. (상태, 조건, 조건 만족 시의 다음상태)
	/// 등록한 순서에 따라 조건을 검색한다
	m_stateMachine->RegistState(E_STATE::IDLE, E_STATE_CONDITION::ENCOUNTER, new CAIStateFollow(E_STATE::FOLLOW));
	m_stateMachine->RegistState(E_STATE::IDLE, E_STATE_CONDITION::HIT, new CAIStateHit(E_STATE::FOLLOW));
	m_stateMachine->RegistState(E_STATE::IDLE, E_STATE_CONDITION::WANDER, new CAIStateWander(E_STATE::IDLE));
	m_stateMachine->RegistState(E_STATE::IDLE, E_STATE_CONDITION::DEFAULT, new CAIStateIdle(E_STATE::IDLE));

	
	m_stateMachine->RegistState(E_STATE::FOLLOW, E_STATE_CONDITION::ARRIVE, new CAIStateAttack(E_STATE::ATTACK));
	m_stateMachine->RegistState(E_STATE::FOLLOW, E_STATE_CONDITION::HIT, new CAIStateHit(E_STATE::FOLLOW));
	m_stateMachine->RegistState(E_STATE::FOLLOW, E_STATE_CONDITION::DEFAULT, new CAIStateFollow(E_STATE::FOLLOW));
	m_stateMachine->RegistState(E_STATE::FOLLOW, E_STATE_CONDITION::END, new CAIStateReturn(E_STATE::RETURN));
	
	m_stateMachine->RegistState(E_STATE::ATTACK, E_STATE_CONDITION::DEFAULT, new CAIStateAttack(E_STATE::ATTACK));
	m_stateMachine->RegistState(E_STATE::ATTACK, E_STATE_CONDITION::ENCOUNTER, new CAIStateFollow(E_STATE::FOLLOW));
	m_stateMachine->RegistState(E_STATE::ATTACK, E_STATE_CONDITION::END, new CAIStateReturn(E_STATE::RETURN));

	m_stateMachine->RegistState(E_STATE::RETURN, E_STATE_CONDITION::DEFAULT, new CAIStateIdle(E_STATE::IDLE));
	
	m_stateMachine->SetCurrentState(E_STATE::IDLE, E_STATE_CONDITION::DEFAULT);
}


CNPC::~CNPC()
{
	SAFE_POOL_DELETE(m_stateMachine, CAIStateMachine);

}


void CNPC::Update()
{
	m_stateMachine->Update();
}


void CNPC::SetAIPlayState(bool bIs_)
{
	m_stateMachine->SetCurrentPlayState(bIs_);
}


bool CNPC::IsValid() const
{
	return true;
}

