#pragma once

#include "PCManager.h"
#include "Session.h"
#include "Singleton.hpp"
#include "VirtualMemoryPool.hpp"


CPCManager::CPCManager()
{
}


CPCManager::~CPCManager()
{
}


CPC* CPCManager::CreatePC(SERIAL_ID serialId_)
{
	// 임시
	static USER_ID temporaryID = 1;
	USER_ID userid = temporaryID++;

	CPC::_StInit init;
	init.userID = temporaryID;
	init.serialID = serialId_;

	CPC* player = NULL;
	if (!GetPoolObject(player, &init))
	{
		// 로그
		return NULL;
	}

	MAP_PC::accessor accessor;
	if (!m_mapPC.insert(accessor, init.serialID))
	{
		// 로그
		return NULL;
	}

	accessor->second = player;

	return accessor->second;
}


CPC* CPCManager::FindPC(SERIAL_ID actorid_)
{
	if (actorid_ == INVALID_SERIAL_ID)
	{
		return NULL;
	}

	MAP_PC::accessor accessor;
	if (!m_mapPC.find(accessor, actorid_))
	{
		// 로그
		return NULL;
	}

	return accessor->second;
}
	

bool CPCManager::DestroyPC(CPC const* pc_)
{
	if (pc_ == NULL)
	{
		return false;
	}

	if (pc_->GetActorType() != E_ACTOR::PC)
	{
		return false;
	}

	if (!BackPoolObject(const_cast<CPC*>(pc_)))
	{
		// 로그
		return false;
	}

	return true;
}


bool CPCManager::DestroyPC(SERIAL_ID actorid_)
{
	MAP_PC::accessor accessor;
	if (!m_mapPC.find(accessor, actorid_))
	{
		// 로그
		return false;
	}

	CPC *pc = accessor->second;
	if (pc == NULL)
	{
		// 로그
		return false;
	}

	if (pc->GetActorType() != E_ACTOR::PC)
	{
		return false;
	}

	if (!BackPoolObject(pc))
	{
		// 로그
		return false;
	}

	return true;
}
