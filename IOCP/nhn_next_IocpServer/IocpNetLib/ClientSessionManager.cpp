#include "ContentsConfig.h"
#include "FastSpinlock.h"
#include "Exception.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "IocpManager.h"


ClientSessionManager* GClientSessionManager = nullptr;

ClientSessionManager::~ClientSessionManager()
{
	for (auto it : mFreeSessionList)
	{
		delete it;
	}
}

void ClientSessionManager::PrepareClientSessions()
{

	m_MaxSessionCount = MAX_CONNECTION;

	for (int i = 0; i < m_MaxSessionCount; ++i)
	{
		ClientSession* client = new ClientSession();
			
		mFreeSessionList.push_back(client);
		m_SessionList.push_back(client);
	}

	printf_s("[DEBUG][%s] m_MaxSessionCount: %d\n", __FUNCTION__, m_MaxSessionCount);
}

void ClientSessionManager::ReturnClientSession(ClientSession* client)
{
	FastSpinlockGuard guard(mLock);

	CRASH_ASSERT(client->mConnected == 0 && client->mRefCount == 0);

	client->SessionReset();

	mFreeSessionList.push_back(client);

	++mCurrentReturnCount;
}

bool ClientSessionManager::AcceptClientSessions()
{
	FastSpinlockGuard guard(mLock);

	while (mCurrentIssueCount - mCurrentReturnCount < m_MaxSessionCount)
	{
		ClientSession* newClient = mFreeSessionList.back();
		mFreeSessionList.pop_back();

		++mCurrentIssueCount;
		
		if (false == newClient->PostAccept()) {
			return false;
		}		
	}


	return true;
}

ClientSession* ClientSessionManager::GetClientSession(const int index)
{
	if (index < 0 || index >= m_MaxSessionCount) {
		return nullptr;
	}

	return m_SessionList[index];
}