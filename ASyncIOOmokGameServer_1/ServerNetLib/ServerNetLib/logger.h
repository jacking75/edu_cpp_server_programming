#include <memory>
#pragma warning(push)
#pragma warning(disable : 4819)
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog\sinks\basic_file_sink.h>
#pragma warning(pop)

namespace NServerNetLib
{
	class Logger
	{
	private:
		std::shared_ptr<spdlog::logger>	m_logger;

	public:
		Logger()
		{
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/OmokGameServerLogFile.txt", true);
			file_sink->set_level(spdlog::level::trace);

			m_logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("multi_sink", { console_sink, file_sink }));

			m_logger->set_level(spdlog::level::debug);
		}

		template <typename... Args>
		void info(const char* message, const Args& ... args)
		{
			m_logger->info(message, args...);
		}

		template <typename... Args>
		void error(const char* message, const Args& ... args)
		{
			m_logger->error(message, args...);
		}
	};
}

