#include <memory>
#pragma warning(push)
#pragma warning(disable : 4819)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)
namespace NServerNetLib
{
	class Logger
	{
	private:
		static Logger* m_instance;
		static std::once_flag m_initFlag;

	public:
		static Logger* getInstance()
		{
			std::call_once(m_initFlag, []() { m_instance = new Logger(); });
			return m_instance;
		}

	private:
		std::shared_ptr<spdlog::logger>	m_loggerConsole;

	public:
		Logger()
		{
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

			m_loggerConsole = std::shared_ptr<spdlog::logger>(new spdlog::logger("console_sink", console_sink));

			m_loggerConsole->set_level(spdlog::level::debug);
		}

		template <typename... Args>
		void info(const char* message, const Args& ... args)
		{
			m_loggerConsole->info(message, args...);
		}

		template <typename... Args>
		void error(const char* message, const Args& ... args)
		{
			m_loggerConsole->error(message, args...);
		}
	};
}

