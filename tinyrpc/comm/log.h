#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>
#include <sstream>
#include <fmt/core.h>  // fmt 库用于格式化支持

#include "tinyrpc/comm/config.h"

namespace tinyrpc {

// 外部日志对象声明
extern std::shared_ptr<spdlog::logger> gRpcLogger;
extern std::shared_ptr<spdlog::logger> gAppLogger;

LogLevel stringToLevel(const std::string& str);
std::string levelToString(LogLevel level);
void initLogger();

// 日志流类
class LoggerStream {
public:
    LoggerStream() : oss() {}

    ~LoggerStream() {
        // 析构时实际输出日志
        if (gRpcLogger) {
            gRpcLogger->info(oss.str());
        }
        if (gAppLogger) {
            gAppLogger->info(oss.str());
        }
    }

    // 支持格式化输出
    template <typename... Args>
    LoggerStream& operator()(const char* format, Args&&... args) {
        oss << fmt::format(format, std::forward<Args>(args)...);
        return *this;
    }

    // 支持流式输出
    template <typename T>
    LoggerStream& operator<<(const T& value) {
        oss << value;  // 构建日志信息
        return *this;
    }

private:
    std::ostringstream oss;  // 用于拼接日志信息的流
};

// 初始化日志器
void initLogger();

// 日志宏定义
#define DebugLog tinyrpc::gRpcLogger->debug("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define InfoLog tinyrpc::gRpcLogger->info("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define WarnLog tinyrpc::gRpcLogger->warn("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define ErrorLog tinyrpc::gRpcLogger->error("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define AppDebugLog tinyrpc::gAppLogger->debug("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define AppInfoLog tinyrpc::gAppLogger->info("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define AppWarnLog tinyrpc::gAppLogger->warn("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

#define AppErrorLog tinyrpc::gAppLogger->error("[{}:{}] ", __FILE__, __LINE__), \
  tinyrpc::LoggerStream()

}  // namespace tinyrpc
