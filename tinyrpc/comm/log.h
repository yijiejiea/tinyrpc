#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>
#include <sstream>

#include "tinyrpc/comm/config.h"

// 跨平台支持
#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
#else
    #include <unistd.h>
#endif

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
        if (gRpcLogger) {
            gRpcLogger->info(oss.str());
        }
        if (gAppLogger) {
            gAppLogger->info(oss.str());
        }
    }

    template <typename T>
    LoggerStream& operator<<(const T& value) {
        oss << value;
        return *this;
    }

private:
    std::ostringstream oss;
};

// 初始化日志器


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
