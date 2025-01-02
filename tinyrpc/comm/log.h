#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
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
  void initLogger(const std::string &log_file_path = "tinyrpc.log", spdlog::level::level_enum log_level = spdlog::level::debug);

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
#define DebugLog tinyrpc::gRpcLogger->debug("[{}:{}] [debug] ", __FILE__, __LINE__), \
                 tinyrpc::LoggerStream(spdlog::level::debug)

#define InfoLog tinyrpc::gRpcLogger->info("[{}:{}] [info] ", __FILE__, __LINE__), \
                tinyrpc::LoggerStream(spdlog::level::info)

#define WarnLog tinyrpc::gRpcLogger->warn("[{}:{}] [warning] ", __FILE__, __LINE__), \
                tinyrpc::LoggerStream(spdlog::level::warn)

#define ErrorLog tinyrpc::gRpcLogger->error("[{}:{}] [error] ", __FILE__, __LINE__), \
                 tinyrpc::LoggerStream(spdlog::level::err)

// 应用程序日志宏定义
#define AppDebugLog tinyrpc::gAppLogger->debug("[{}:{}] [debug] ", __FILE__, __LINE__), \
                    tinyrpc::LoggerStream(spdlog::level::debug)

#define AppInfoLog tinyrpc::gAppLogger->info("[{}:{}] [info] ", __FILE__, __LINE__), \
                   tinyrpc::LoggerStream(spdlog::level::info)

#define AppWarnLog tinyrpc::gAppLogger->warn("[{}:{}] [warning] ", __FILE__, __LINE__), \
                   tinyrpc::LoggerStream(spdlog::level::warn)

#define AppErrorLog tinyrpc::gAppLogger->error("[{}:{}] [error] ", __FILE__, __LINE__), \
                    tinyrpc::LoggerStream(spdlog::level::err)

} // namespace tinyrpc
