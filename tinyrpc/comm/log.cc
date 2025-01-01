#include "log.h"
namespace tinyrpc
{

  std::shared_ptr<spdlog::logger> gRpcLogger;
  std::shared_ptr<spdlog::logger> gAppLogger;

  std::string levelToString(LogLevel level)
  {
    std::string re = "DEBUG";
    switch (level)
    {
    case DEBUG:
      re = "DEBUG";
      return re;

    case INFO:
      re = "INFO";
      return re;

    case WARN:
      re = "WARN";
      return re;

    case ERROR:
      re = "ERROR";
      return re;
    case NONE:
      re = "NONE";

    default:
      return re;
    }
  }

  LogLevel stringToLevel(const std::string &str)
  {
    if (str == "DEBUG")
      return LogLevel::DEBUG;

    if (str == "INFO")
      return LogLevel::INFO;

    if (str == "WARN")
      return LogLevel::WARN;

    if (str == "ERROR")
      return LogLevel::ERROR;

    if (str == "NONE")
      return LogLevel::NONE;

    return LogLevel::DEBUG;
  }

  void initLogger()
  {
    // 初始化RPC日志器，控制台输出
    gRpcLogger = spdlog::stdout_color_mt("rpc");
    gRpcLogger->set_level(spdlog::level::debug);  // 设置默认日志级别为 debug
    gRpcLogger->flush_on(spdlog::level::info);

    // 初始化应用日志器，控制台输出
    gAppLogger = spdlog::stdout_color_mt("app");
    gAppLogger->set_level(spdlog::level::debug);  // 设置默认日志级别为 debug
    gAppLogger->flush_on(spdlog::level::info);
  }
}
