#pragma once
#include <ctime>
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"


namespace OmokServerLib
{
	struct ConnectedUser
	{
		bool m_IsLoginSuccess = false;
		time_t m_ConnectedTime = 0;

		void Clear()
		{
			m_IsLoginSuccess = false;
			m_ConnectedTime = 0;
		}
	};


	class ConnectedUserManager
	{
	public:
		ConnectedUserManager() = default;
		~ConnectedUserManager() = default;

		void Init(const int maxSessionCount, NServerNetLib::TcpNetwork* pNetwork, NServerNetLib::Logger* pLogger)
		{
			m_pRefLogger = pLogger;
			m_pRefNetwork = pNetwork;

			for (int i = 0; i < maxSessionCount; ++i)
			{
				ConnectedUserList.emplace_back(ConnectedUser());
			}
		}

		void SetConnectSession(const int sessionIndex)
		{
			time(&ConnectedUserList[sessionIndex].m_ConnectedTime);
		}

		void SetLogin(const int sessionIndex)
		{
			ConnectedUserList[sessionIndex].m_IsLoginSuccess = true;
		}

		bool CheckUserLogin(const int sessionIndex)
		{
			if (ConnectedUserList[sessionIndex].m_IsLoginSuccess == true)
			{
				return true;
			}

			return false;
		}

		void SetDisConnectSession(const int sessionIndex)
		{
			ConnectedUserList[sessionIndex].Clear();
		}

		void LoginCheck()
		{
			auto curTime = std::chrono::system_clock::now();
			auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_LatestLoginCheckTime);

			if (diffTime.count() < checkIndexTime)
			{
				return;
			}
			else
			{
				m_LatestLoginCheckTime = curTime;
			}

			auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

			const auto maxSessionCount = (int)ConnectedUserList.size();

			if (m_LatestLogincheckIndex >= maxSessionCount) 
			{
				m_LatestLogincheckIndex = -1;
			}

			++m_LatestLogincheckIndex;

			auto lastCheckIndex = m_LatestLogincheckIndex + checkIndexRangeCount;
			if (lastCheckIndex > maxSessionCount)
			{
				lastCheckIndex = maxSessionCount;
			}

			for (; m_LatestLogincheckIndex < lastCheckIndex; ++m_LatestLogincheckIndex)
			{
				auto i = m_LatestLogincheckIndex;

				if (ConnectedUserList[i].m_ConnectedTime == 0 || ConnectedUserList[i].m_IsLoginSuccess == true)
				{
					continue;
				}

				auto diff = curSecTime - ConnectedUserList[i].m_ConnectedTime;				
				if (diff >= LoginWaitTime)
				{
					m_pRefLogger->error("{} | Login Wait Time Over. sessionIndex [{}].", __FUNCTION__, i);
					//m_pRefNetwork->CloseSocket(i);
				}
			}
		}


	private:
		NServerNetLib::Logger* m_pRefLogger;

		NServerNetLib::TcpNetwork* m_pRefNetwork;

		std::vector<ConnectedUser> ConnectedUserList;

		std::chrono::system_clock::time_point m_LatestLoginCheckTime = std::chrono::system_clock::now();

		int m_LatestLogincheckIndex = -1;

		const int checkIndexTime = 100;

		const int checkIndexRangeCount = 200;

		const int LoginWaitTime = 300;
	};

}