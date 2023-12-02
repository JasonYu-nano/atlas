// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "spdlog/spdlog.h"

#include "core_def.hpp"

using spdlog::level::level_enum;

namespace atlas
{

CORE_API std::unique_ptr<spdlog::logger> CreateLogger(const std::string& logger_name, const std::string& file_to_save);

template<typename... Args>
static void Log(spdlog::logger& logger, level_enum level, fmt::format_string<Args...> fmt, Args&&... args)
{
    logger.log(level, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
static void Log(spdlog::logger& logger, spdlog::source_loc src, level_enum level, fmt::format_string<Args...> fmt, Args&&... args)
{
    logger.log(src, level, fmt, std::forward<Args>(args)...);
}

#define DEFINE_LOGGER_DETAILED(logger_name, file_to_save)   \
class Logger_##logger_name                                  \
{                                                           \
public:                                                     \
    Logger_##logger_name() = delete;                        \
    static std::unique_ptr<spdlog::logger> logger_;         \
};                                                          \
std::unique_ptr<spdlog::logger> Logger_##logger_name::logger_ = CreateLogger(#logger_name, #file_to_save)

#define DEFINE_LOGGER(logger_name) DEFINE_LOGGER_DETAILED(logger_name, game)

#define LOG_INTERNAL(logger, level, fmt, ...) { Log(*Logger_##logger::logger_.get(), spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, level, fmt, ## __VA_ARGS__); }
#define LOG_INTERNAL_NO_SOURCE(logger, level, fmt, ...) { Log(*Logger_##logger::logger_.get(), level, fmt, ## __VA_ARGS__); }

#define LOG_TRACE(logger, fmt, ...)  LOG_INTERNAL_NO_SOURCE(logger, level_enum::trace, fmt, ## __VA_ARGS__)
#define LOG_DEBUG(logger, fmt, ...)  LOG_INTERNAL_NO_SOURCE(logger, level_enum::debug, fmt, ## __VA_ARGS__)
#define LOG_INFO(logger, fmt, ...)  LOG_INTERNAL_NO_SOURCE(logger, level_enum::info, fmt, ## __VA_ARGS__)
#define LOG_WARN(logger, fmt, ...)  LOG_INTERNAL(logger, level_enum::warn, fmt, ## __VA_ARGS__)
#define LOG_ERROR(logger, fmt, ...)  LOG_INTERNAL(logger, level_enum::err, fmt, ## __VA_ARGS__)
#define LOG_CRITICAL(logger, fmt, ...)  LOG_INTERNAL(logger, level_enum::critical, fmt, ## __VA_ARGS__)

#define CLOG_TRACE(expression, logger, fmt, ...) if (expression) LOG_TRACE(logger, fmt, ## __VA_ARGS__)
#define CLOG_DEBUG(expression, logger, fmt, ...) if (expression) LOG_DEBUG(logger, fmt, ## __VA_ARGS__)
#define CLOG_INFO(expression, logger, fmt, ...) if (expression) LOG_INFO(logger, fmt, ## __VA_ARGS__)
#define CLOG_WARN(expression, logger, fmt, ...) if (expression) LOG_WARN(logger, fmt, ## __VA_ARGS__)
#define CLOG_ERROR(expression, logger, fmt, ...) if (expression) LOG_ERROR(logger, fmt, ## __VA_ARGS__)
#define CLOG_CRITICAL(expression, logger, fmt, ...) if (expression) LOG_CRITICAL(logger, fmt, ## __VA_ARGS__)

}