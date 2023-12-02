// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "log/logger.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace atlas
{
std::unique_ptr<spdlog::logger> CreateLogger(const std::string &logger_name, const std::string &file_to_save)
{
    std::vector<spdlog::sink_ptr> sinks;

    auto color_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    color_sink->set_color(level_enum::info, color_sink->cyan);
    color_sink->set_color(level_enum::warn, color_sink->yellow);
    color_sink->set_color(level_enum::err, color_sink->red);
    color_sink->set_color(level_enum::critical, color_sink->red_bold);
    sinks.push_back(color_sink);

    std::unique_ptr<spdlog::logger> logger = std::make_unique<spdlog::logger>(logger_name, begin(sinks), end(sinks));
    logger->set_pattern("[%n] [%D] [%H:%M:%S] [%l] [%s:%#] %v%$");
    return logger;
}
}