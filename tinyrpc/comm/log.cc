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

  void initLogger(const std::string& log_file_path, spdlog::level::level_enum log_level) {
    // 创建控制台输出（有颜色）
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(log_level);  // 控制台日志级别

    // 创建文件输出（日志保存到文件）
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_path, true);
    file_sink->set_level(log_level);  // 文件日志级别

    // 创建 gRpcLogger
    gRpcLogger = std::make_shared<spdlog::logger>("rpc_logger", spdlog::sinks_init_list{console_sink, file_sink});
    gRpcLogger->set_level(log_level);  // 全局日志级别

    // 创建 gAppLogger，使用不同的文件或输出
    auto app_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("app_" + log_file_path, true);
    gAppLogger = std::make_shared<spdlog::logger>("app_logger", app_file_sink);
    gAppLogger->set_level(log_level);  // 可以设置不同的日志级别
}

}
