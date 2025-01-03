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

  std::string getBinaryName()
  {
    char buffer[256];
    std::memset(buffer, 0, sizeof(buffer));

    // 获取当前二进制文件路径
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
      buffer[len] = '\0';
    }

    std::string path(buffer);
    return std::filesystem::path(path).stem().string(); // 获取二进制文件名，不带路径和扩展名
  }

  std::string getCurrentTimeString()
  {
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
    localtime_r(&time_t_now, &tm_now);

    // 格式化时间为字符串
    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
  }

  // 初始化日志函数
  void initLogger(
      std::optional<LogLevel> log_level, // 使用自定义的 LogLevel 枚举
      std::optional<std::string> log_file_path,
      std::optional<size_t> max_file_size,
      std::optional<size_t> max_files)
  {
    // 使用 value_or 获取可选参数的值
    spdlog::level::level_enum level = mapLogLevel(log_level.value_or(LogLevel::DEBUG));

    // 自动生成日志文件名，使用默认文件名或者传入的文件名
    std::string log_file = log_file_path.value_or(getBinaryName() + "_log_" + getCurrentTimeString() + ".log");

    // 获取最大文件大小和文件数量
    size_t file_size = max_file_size.value_or(10 * 1024 * 1024); // 默认 10MB
    size_t files = max_files.value_or(5);                        // 默认最多 5 个文件
    // 创建控制台输出（有颜色）
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(level); // 控制台日志级别

    // 创建滚动日志文件输出（文件大小达到限制时创建新文件）
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, file_size, files);
    rotating_sink->set_level(level); // 文件日志级别

    // 创建 gRpcLogger
    gRpcLogger = std::make_shared<spdlog::logger>("rpc_logger", spdlog::sinks_init_list{console_sink, rotating_sink});
    gRpcLogger->set_level(level); // 全局日志级别

    // 创建 gAppLogger，使用不同的文件或输出
    auto app_rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("app_" + log_file, file_size, files); // 使用相同的日志文件路径，但前缀为 "app_"
    gAppLogger = std::make_shared<spdlog::logger>("app_logger", app_rotating_sink);
    gAppLogger->set_level(level); // 可以设置不同的日志级别
  }

  spdlog::level::level_enum mapLogLevel(LogLevel log_level)
  {
    switch (log_level)
    {
    case LogLevel::DEBUG:
      return spdlog::level::debug;
    case LogLevel::INFO:
      return spdlog::level::info;
    case LogLevel::WARN:
      return spdlog::level::warn;
    case LogLevel::ERROR:
      return spdlog::level::err;
    case LogLevel::NONE:
      return spdlog::level::off;
    default:
      return spdlog::level::debug; // 默认值
    }
  }
}