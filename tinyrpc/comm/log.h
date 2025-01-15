#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <memory>
#include <optional>
#include <string>
#include <filesystem>
#include <sstream>
#include <fmt/core.h> // fmt 库用于格式化支持

#include "tinyrpc/comm/config.h"

namespace tinyrpc
{

  // 外部日志对象声明
  extern std::shared_ptr<spdlog::logger> gRpcLogger;
  extern std::shared_ptr<spdlog::logger> gAppLogger;

  LogLevel stringToLevel(const std::string &str);
  std::string levelToString(LogLevel level);
  // 初始化日志函数
  void initLogger(
      std::optional<LogLevel> log_level = std::optional<LogLevel>(DEBUG),
      std::optional<std::string> log_file_path = std::optional<std::string>(),
      std::optional<size_t> max_file_size = std::optional<size_t>(10 * 1024 * 1024), // 默认 10MB
      std::optional<size_t> max_files = std::optional<size_t>(5)                     // 默认最多 5 个文件
  );

  spdlog::level::level_enum mapLogLevel(LogLevel log_level);

  // 日志流类
  class LoggerStream
  {
  public:
    // 接受一个日志级别来构造
    LoggerStream(spdlog::level::level_enum level) : oss(), level(level) {}

    // 析构时输出日志
    ~LoggerStream()
    {
      if (gRpcLogger)
      {
        gRpcLogger->log(level, oss.str());
      }
    }

    // 支持格式化输出
    template <typename... Args>
    LoggerStream &operator()(const char *format, Args &&...args)
    {
      oss << fmt::format(format, std::forward<Args>(args)...);
      return *this;
    }

    // 支持流式输出
    template <typename T>
    LoggerStream &operator<<(const T &value)
    {
      oss << value; // 构建日志信息
      return *this;
    }

  private:
    std::ostringstream oss;          // 用于拼接日志信息的流
    spdlog::level::level_enum level; // 日志级别
  };

  // 日志宏定义
  // RPC 日志宏定义
  #define DebugLog tinyrpc::Logger::getInstance().getRpcLogger()->debug("[{}:{}]", __FILE__, __LINE__), \
                   tinyrpc::LoggerStream(spdlog::level::debug)

  #define InfoLog tinyrpc::Logger::getInstance().getRpcLogger()->info("[{}:{}] ", __FILE__, __LINE__), \
                  tinyrpc::LoggerStream(spdlog::level::info)

  #define WarnLog tinyrpc::Logger::getInstance().getRpcLogger()->warn("[{}:{}]", __FILE__, __LINE__), \
                  tinyrpc::LoggerStream(spdlog::level::warn)

  #define ErrorLog tinyrpc::Logger::getInstance().getRpcLogger()->error("[{}:{}]", __FILE__, __LINE__), \
                   tinyrpc::LoggerStream(spdlog::level::err)

  // 应用程序日志宏定义
  #define AppDebugLog tinyrpc::Logger::getInstance().getAppLogger()->debug("[{}:{}] [debug] ", __FILE__, __LINE__), \
                      tinyrpc::LoggerStream(spdlog::level::debug)

  #define AppInfoLog tinyrpc::Logger::getInstance().getAppLogger()->info("[{}:{}] [info] ", __FILE__, __LINE__), \
                     tinyrpc::LoggerStream(spdlog::level::info)

  #define AppWarnLog tinyrpc::Logger::getInstance().getAppLogger()->warn("[{}:{}] [warning] ", __FILE__, __LINE__), \
                     tinyrpc::LoggerStream(spdlog::level::warn)

  #define AppErrorLog tinyrpc::Logger::getInstance().getAppLogger()->error("[{}:{}] [error] ", __FILE__, __LINE__), \
                      tinyrpc::LoggerStream(spdlog::level::err)

  // 单例模式的 Logger 类
  class Logger {
  public:
    static Logger& getInstance() {
      static Logger instance;
      return instance;
    }

    std::shared_ptr<spdlog::logger> getRpcLogger() {
      if (!gRpcLogger) {
        initLogger();
      }
      return gRpcLogger;
    }

    std::shared_ptr<spdlog::logger> getAppLogger() {
      if (!gAppLogger) {
        initLogger();
      }
      return gAppLogger;
    }

  private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
  };

} // namespace tinyrpc
