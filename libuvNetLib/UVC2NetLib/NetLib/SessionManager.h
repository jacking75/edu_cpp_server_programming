#pragma once

#include <vector>
#include <deque>
#include <thread>

#include "Session.h"
#include "CommonDef.h"

namespace UVC2NetLibz
{
	const uint64_t RE_USE_SESSION_WAITE_TIMESEC = 3;

	struct SessionIndexContext
	{
		uint32_t Index = 0;
		uint64_t ReUseAbleTimeSec = 0;
	};

	class SessionManager
	{
	public:
		SessionManager() = default;
		~SessionManager() = default;
		
		bool PrepareSessionPool(const uint32_t maxSessionCount_)
		{
			for (uint32_t i = 0; i < maxSessionCount_; ++i)
			{
				auto session = new Session;
				session->Init(i);
				
				mSessions.push_back(session);
				mFreeSessionindexs.emplace_back(SessionIndexContext{ i });
			}			
			
			mMaxSessionCount = maxSessionCount_;


			mIsSenderRun = true;
			mSendThread = std::thread([this]() { SendThread(); });
			
			return true;
		}

		void Dispose()
		{
			LogFuncPtr((int)LogLevel::info, "SessionManager::Dispose - Start");

			mIsSenderRun = false;
			if (mSendThread.joinable())
			{
				mSendThread.join();
			}


			for (auto pSession : mSessions)
			{
				pSession->Dispose();
				delete pSession;
			}

			LogFuncPtr((int)LogLevel::info, "SessionManager::Dispose - Complete");
		}

		Session* GetSessionObj(const uint32_t sessionIndex_)
		{
			if (sessionIndex_ >= mMaxSessionCount) { return nullptr; }
			return mSessions[sessionIndex_];
		}

		Session* GetSessionObj(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_)
		{
			if (sessionIndex_ >= mMaxSessionCount) { return nullptr; }
			if (mSessions[sessionIndex_]->GetUniqueId() != sessionUniqueId_) { return nullptr; }

			return mSessions[sessionIndex_];
		}

		Session* IssueClientSession(const uint64_t curSec_)
		{
			if (mFreeSessionindexs.empty()) { return nullptr; }

			auto indexContext = mFreeSessionindexs.front();

			if (indexContext.ReUseAbleTimeSec > curSec_) { return nullptr; }

			auto newSession = GetSessionObj(indexContext.Index);
			mFreeSessionindexs.pop_front();

			//newSessions는 절대 null 객체가 아니다!!!
			return newSession;
		}

		void ReturnSession(const int sessionIndex_, const uint64_t curSec_)
		{
			mFreeSessionindexs.emplace_back(SessionIndexContext{ 
									(uint32_t)sessionIndex_, 
									(curSec_ + RE_USE_SESSION_WAITE_TIMESEC) });
		}

				

	private:
		void SendThread()
		{
			LogFuncPtr((int)LogLevel::info, "SendThread() - Start");

			while (mIsSenderRun)
			{
				for (auto session : mSessions)
				{
					if (session->IsEnableSend() == false)
					{
						continue;
					}

					session->SendIO();
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(8));
			}

			LogFuncPtr((int)LogLevel::info, "SendThread() - End");
		}

		//FastSpinlock mLock;

		uint32_t mMaxSessionCount = 0;
		uint64_t mCurrentIssueCount = 0;
		uint64_t mCurrentReturnCount = 0;

		std::deque<SessionIndexContext> mFreeSessionindexs;
		std::vector<Session*> mSessions;

		bool		mIsSenderRun = false;
		std::thread	mSendThread;
	};
}