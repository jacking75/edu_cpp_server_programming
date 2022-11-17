#pragma once

#include <thread>



namespace NetLib
{
	class Performance
	{
	public:
		Performance() {}
		
		~Performance()
		{
			if (m_IsCheckPerformance)
			{
				SetCheckPerformance(FALSE);
				m_PerformanceThread.join();
			}
		}

	public:

		void Start(int milliseconds = 0)
		{
			m_CheckPerformanceMilliseconds = milliseconds;

			if (m_CheckPerformanceMilliseconds > 0)
			{
				SetCheckPerformance(TRUE);
				m_PerformanceThread = std::thread(&Performance::CheckPerformanceThread, this);
			}
		}

		void CheckPerformanceThread(void)
		{
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(m_CheckPerformanceMilliseconds));
				if (!m_IsCheckPerformance)
				{
					return;
				}

				char logmsg[64] = { 0, };
				sprintf_s(logmsg, "Process Packet Count:%d, Per:%d", m_PacketProcessCount, m_CheckPerformanceMilliseconds);
				LogFuncPtr((int)LogLevel::eDEBUG, logmsg);

				ResetPacketProcessCount();
			}
		}

		int IncrementPacketProcessCount(void) { return m_IsCheckPerformance ? InterlockedIncrement(&m_PacketProcessCount) : 0; }
		
	private:
		void SetCheckPerformance(BOOL isCheckPerformance) { InterlockedExchange(reinterpret_cast<LPLONG>(&m_IsCheckPerformance), isCheckPerformance); }
		void ResetPacketProcessCount(void) { InterlockedExchange(&m_PacketProcessCount, 0); }

	private:
		std::thread m_PerformanceThread;
		long m_PacketProcessCount = 0;
		BOOL m_IsCheckPerformance = FALSE;
		int m_CheckPerformanceMilliseconds = 0;
	};
}