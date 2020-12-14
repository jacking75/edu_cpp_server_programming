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
		ConnectedUserManager() {}
		~ConnectedUserManager() {}

		void Init(const int maxSessionCount, NServerNetLib::TcpNetwork* pNetwork, NServerNetLib::ServerConfig pConfig, NServerNetLib::Logger* pLogger)
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

		void SetDisConnectSession(const int sessionIndex)
		{
			ConnectedUserList[sessionIndex].Clear();
		}

		void LoginCheck()
		{
			auto curTime = std::chrono::system_clock::now();
			auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

			const auto maxSessionCount = (int)ConnectedUserList.size();

			if (m_LatestLogincheckIndex >= maxSessionCount)
			{
				m_LatestLogincheckIndex = -1;
			}

			++m_LatestLogincheckIndex;

			for (; m_LatestLogincheckIndex < maxSessionCount; ++m_LatestLogincheckIndex)
			{
				auto i = m_LatestLogincheckIndex;

				if (ConnectedUserList[i].m_ConnectedTime == 0 || ConnectedUserList[i].m_IsLoginSuccess == true)
				{
					continue;
				}

				auto diff = curSecTime - ConnectedUserList[i].m_ConnectedTime;
				if (diff >= 300)
				{
					m_pRefLogger->error("{} | Login Wait Time Over. sessionIndex [{}].", __FUNCTION__, i);
					m_pRefNetwork->ForcingClose(i);
				}
			}
		}

	private:
		NServerNetLib::Logger* m_pRefLogger;
		NServerNetLib::TcpNetwork* m_pRefNetwork;

		std::vector<ConnectedUser> ConnectedUserList;

		int m_LatestLogincheckIndex = -1;
	};

}