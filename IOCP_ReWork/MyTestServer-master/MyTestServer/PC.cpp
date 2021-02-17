#pragma once
#include "PC.h"
#include "Actor.h"
#include "Define.h"
#include "Session.h"


CPC::CPC()
	: CActor(E_ACTOR::PC)
{
}


CPC::~CPC()
{
}


void CPC::SetSession(CSession* session_)
{
	m_pSession = session_;
}


bool CPC::Leave(void* initParam_)
{
	if (initParam_ == NULL)
		return false;

	_StInit *init = reinterpret_cast<_StInit*>(initParam_);
	m_userID = init->userID;
	m_actorID = init->serialID;

	// юс╫ц
	SetPosition(100, 100, 140);

	return true;
}


bool CPC::Enter()
{
	m_pSession = NULL;
	m_userID = INVALID_USER_ID;

	return true;
}


void CPC::Update()
{
	m_pSession->FlushAllSend();
}

void CPC::SetAIPlayState(bool)
{
}

bool CPC::IsValid() const
{
	if (IsUsed() && !IsCacheState())
	{
		return true;
	}
	return false;
}



